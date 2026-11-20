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
                int start_gold = player->gold;
                
                while (running) {
                    map_print(m, player);
                    printf("Время: %02d:00 | Последнее событие: %s\n", g_hour, g_last_event);
                    printf("Управление: W/A/S/D — ход/атака, I — инвентарь, E — подобрать, Q — выход\n");
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
                        int picked = 0;
                        for (int i = 0; i < m->drop_count; i++) {
                            if (m->drops[i].is_alive && m->drops[i].x == m->hero_x && m->drops[i].y == m->hero_y) {
                                if (m->drops[i].item_id == ITEM_GOLD) {
                                    player->gold += 1;
                                    sprintf(g_last_event, "Вы подобрали 1 золотой.");
                                } else {
                                    int slot = -1;
                                    for (int j = 0; j < INV_SIZE; j++) {
                                        if (player->inventory[j] == ITEM_EMPTY) { slot = j; break; }
                                    }
                                    if (slot != -1) {
                                        player->inventory[slot] = m->drops[i].item_id;
                                        sprintf(g_last_event, "Вы подобрали: %s", g_item_names[m->drops[i].item_id]);
                                    } else {
                                        sprintf(g_last_event, "Рюкзак полон!");
                                        picked = 1; break;
                                    }
                                }
                                m->drops[i].is_alive = 0;
                                picked = 1; break;
                            }
                        }
                        if (!picked) sprintf(g_last_event, "Здесь нечего подбирать.");
                        continue;
                    }

                    if (dir != 'W' && dir != 'A' && dir != 'S' && dir != 'D') {
                        sprintf(g_last_event, "Неизвестная команда.");
                        continue;
                    }

                    // --- UPDATE ---
                    int nx = m->hero_x, ny = m->hero_y;
                    if (dir == 'W') ny--;
                    if (dir == 'S') ny++;
                    if (dir == 'A') nx--;
                    if (dir == 'D') nx++;

                    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) {
                        sprintf(g_last_event, "Там край карты.");
                        entity_update_critters(m->entities, m->entity_count, m->map, m->width, m->height, m->hero_x, m->hero_y);
                        continue;
                    }

                    int interacted = entity_interact(m->entities, m->entity_count, player, nx, ny, m->drops, &m->drop_count, m->map, m->width, m->height);

                    if (!interacted) {
                        int moved = map_move(m, dir);
                        if (moved) {
                            player->stamina--;
                            if (player->stamina < 0) player->stamina = 0;
                            
                            if (m->map[m->hero_y][m->hero_x] == TOWER_STAIR_UP) {
                                sprintf(g_last_event, "Вы поднялись на этаж %d!", m->floor + 1);
                                int reward = 10 * m->floor;
                                player->gold += reward;
                                player->stamina -= 5;
                                if (player->stamina < 0) player->stamina = 0;
                                g_hour++; if (g_hour >= 24) { g_hour -= 24; g_day++; }
                                
                                int next_floor = m->floor + 1;
                                map_free(&m);
                                m = map_create(next_floor);
                            } else {
                                sprintf(g_last_event, "Вы сделали шаг.");
                            }
                        } else {
                            char cell = m->map[ny][nx];
                            if (cell == TOWER_WALL) {
                                const char *phrases[] = {"Ой! Там стена.", "Ай! Ушибся.", "Тут непролазная чаща!"};
                                sprintf(g_last_event, "%s", phrases[rand() % 3]);
                            }
                            else if (cell == TOWER_MIRROR || cell == TOWER_MIRROR_H) 
                                sprintf(g_last_event, "Зеркало не пускает.");
                        }
                    }

                    // ВСЕГДА обновляем зверьков в конце хода
                    entity_update_critters(m->entities, m->entity_count, m->map, m->width, m->height, m->hero_x, m->hero_y);

                    if (player->hp <= 0 || player->stamina <= 0) {
                        printf("Вы упали без сил! Эвакуация...\n");
                        int loss = player->gold / 2;
                        player->gold -= loss;
                        
                        FILE *f = fopen("diary.txt", "a");
                        if (f) {
                            fprintf(f, "--- Отчет о походе (Обморок) ---\n");
                            fprintf(f, "День %d, этаж %d\n", g_day, m->floor);
                            fprintf(f, "Потеряно золота: %d\n\n", loss);
                            fclose(f);
                        }
                        
                        g_day++; g_hour = 8;
                        player->hp = player->max_hp;
                        player->stamina = player->max_stamina;
                        running = 0;
                    }
                }
                
                if (player->hp > 0 && player->stamina > 0) {
                    FILE *f = fopen("diary.txt", "a");
                    if (f) {
                        fprintf(f, "--- Отчет о походе ---\n");
                        fprintf(f, "День %d, этаж %d\n", g_day, m->floor);
                        fprintf(f, "Заработано золота: %d\n\n", player->gold - start_gold);
                        fclose(f);
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