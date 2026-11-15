#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "player.h"
#include "map.h"

int g_day = 1;
int g_hour = 8;

void time_menu(Player **p) {
    int choice;
    while (1) {
        printf("\n--- Время ---\n");
        printf("Сейчас: День %d, %02d:00\n", g_day, g_hour);
        printf("[1] Посмотреть на часы\n");
        printf("[2] Ждать\n");
        printf("[3] Лечь спать\n");
        printf("[0] Назад\n");
        printf("Выбор: ");
        if (!read_int(&choice)) continue;

        if (choice == 0) break;
        
        if (choice == 1) {
            printf("Текущее время: День %d, %02d:00\n", g_day, g_hour);
        } else if (choice == 2) {
            int hours;
            printf("Сколько часов ждать? ");
            if (!read_int(&hours) || hours < 1) { printf("Ошибка.\n"); continue; }
            if (g_hour + hours >= 26) { 
                printf("Нельзя ждать до 02:00 ночи! Нужно спать.\n");
            } else {
                g_hour += hours;
                if (g_hour >= 24) { g_hour -= 24; g_day++; }
                printf("Прошло %d ч. Время: День %d, %02d:00\n", hours, g_day, g_hour);
            }
        } else if (choice == 3) {
            if (!*p) { printf("Сначала создайте персонажа.\n"); continue; }
            g_day++; g_hour = 8;
            (*p)->hp = (*p)->max_hp;
            (*p)->stamina = (*p)->max_stamina;
            printf("Вы поспали. Время: День %d, 08:00. HP и стамина восстановлены.\n", g_day);
        }
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    load_item_names();
    
    Player *player = NULL;
    int choice;

    while (1) {
        printf("\n===== Главное меню =====\n");
        printf("[1] Создать / пересоздать персонажа\n");
        printf("[2] Карточка персонажа\n");
        printf("[3] Время и отдых\n");
        printf("[4] Управление снаряжением\n");
        printf("[10] Отправиться в приключение\n");
        printf("[0] Выход\n");
        printf("Выбор: ");

        if (!read_int(&choice)) continue;

        switch (choice) {
            case 0:
                if (player) player_free(&player);
                return 0;
            case 1:
                if (player) {
                    printf("Персонаж уже существует. Пересоздать? (1 - да, 0 - нет): ");
                    int ans;
                    read_int(&ans);
                    if (!ans) break;
                    player_free(&player);
                }
                player = player_create();
                break;
            case 2:
                if (!player) { printf("Сначала создайте персонажа!\n"); break; }
                player_print_card(player);
                break;
            case 3:
                time_menu(&player);
                break;
            case 4:
                if (!player) { printf("Сначала создайте персонажа!\n"); break; }
                player_equip_menu(player);
                break;
            case 10: {
                if (!player) { printf("Сначала создайте персонажа!\n"); break; }
                if (player->stamina <= 0) { printf("У вас нет сил идти в приключение!\n"); break; }
                if (g_hour == 2) { printf("Сейчас 02:00 ночи, фермер должен спать!\n"); break; }
                
                Map *m = map_create(1);
                if (!m) break;
                
                int running = 1;
                while (running) {
                    map_print(m, player);
                    printf("Время: %02d:00\n", g_hour);
                    printf("Управление: W/A/S/D — ходьба, I — инвентарь, E — подобрать, Q — выход (на D)\n");
                    printf("Ход: ");
                    
                    char buf[16];
                    if (!fgets(buf, sizeof(buf), stdin)) break;
                    char dir = buf[0];
                    if (dir >= 'a' && dir <= 'z') dir -= 32;

                    if (dir == 'Q') {
                        if (m->map[m->hero_y][m->hero_x] == TOWER_STAIR_DOWN) {
                            printf("Вы вернулись домой.\n");
                            running = 0;
                        } else {
                            printf("Выход только на лестнице вниз (D).\n");
                        }
                        continue;
                    }

                    if (dir == 'I') {
                        player_print_card(player);
                        continue;
                    }

                    if (dir == 'E') {
                        if (map_pickup(m)) {
                            int slot = -1;
                            for (int i = 0; i < INV_SIZE; i++) {
                                if (player->inventory[i] == ITEM_EMPTY) { slot = i; break; }
                            }
                            if (slot != -1) {
                                player->inventory[slot] = ITEM_STONE;
                                printf("Подобрали камень в слот %d!\n", slot);
                            } else {
                                printf("Рюкзак полон!\n");
                                m->map[m->hero_y][m->hero_x] = TOWER_STONE_BROKEN;
                            }
                        } else {
                            printf("Здесь нечего подбирать.\n");
                        }
                        continue;
                    }

                    if (dir != 'W' && dir != 'A' && dir != 'S' && dir != 'D') {
                        printf("Неизвестная команда.\n");
                        continue;
                    }

                    int moved = map_move(m, dir);
                    if (moved) {
                        player->stamina--;
                        if (player->stamina < 0) player->stamina = 0;
                        
                        if (m->map[m->hero_y][m->hero_x] == TOWER_STAIR_UP) {
                            printf("Переход на новый этаж!\n");
                            int reward = 10 * m->floor;
                            player->gold += reward;
                            printf("Награда за этаж: +%d золота.\n", reward);
                            
                            player->stamina -= 5;
                            if (player->stamina < 0) player->stamina = 0;
                            g_hour++; if (g_hour >= 24) { g_hour -= 24; g_day++; }
                            
                            int next_floor = m->floor + 1;
                            map_free(&m);
                            m = map_create(next_floor);
                            printf("Память старой карты очищена, создан этаж %d.\n", next_floor);
                        }

                        if (player->stamina <= 0) {
                            printf("Вы упали без сил! Эвакуация...\n");
                            int loss = player->gold / 2;
                            player->gold -= loss;
                            printf("Потеряно золота: %d\n", loss);
                            running = 0;
                        }
                    } else {
                        int nx = m->hero_x, ny = m->hero_y;
                        if (dir == 'W') ny--;
                        if (dir == 'S') ny++;
                        if (dir == 'A') nx--;
                        if (dir == 'D') nx++;
                        if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                            char next_cell = m->map[ny][nx];
                            if (next_cell == TOWER_WALL) printf("Там стена.\n");
                            else if (next_cell == TOWER_ROCK) printf("Там камень.\n");
                            else if (next_cell == TOWER_MIRROR || next_cell == TOWER_MIRROR_H) printf("Зеркало не пускает.\n");
                        }
                    }
                }
                if (m) map_free(&m);
                break;
            }
            default: printf("Нет такого пункта.\n");
        }
    }
    return 0;
}