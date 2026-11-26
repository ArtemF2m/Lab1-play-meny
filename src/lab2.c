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

void action_pickup(Map *m, Player *p) {
    for (int i = 0; i < m->drop_count; i++) {
        if (m->drops[i].is_alive && m->drops[i].x == m->hero_x && m->drops[i].y == m->hero_y) {
            int slot = -1;
            for (int j = 0; j < INV_SIZE; j++) {
                if (p->inventory[j] == ITEM_EMPTY) { slot = j; break; }
            }
            if (slot != -1) {
                p->inventory[slot] = m->drops[i].item_id;
                if (m->drops[i].item_id == ITEM_GOLD) {
                    sprintf(g_last_event, "Вы подобрали золото в рюкзак (G).");
                } else {
                    sprintf(g_last_event, "Вы подобрали: %s", g_item_names[m->drops[i].item_id]);
                }
                m->drops[i].is_alive = 0;
            } else {
                sprintf(g_last_event, "Рюкзак полон!");
            }
            return;
        }
    }
    sprintf(g_last_event, "Здесь нечего подбирать.");
}

void action_drink_tea(Player *p) {
    int found_tea = -1;
    for (int i = 0; i < INV_SIZE; i++) {
        if (p->inventory[i] == ITEM_TEA_BLACK || 
            p->inventory[i] == ITEM_TEA_GREEN || 
            p->inventory[i] == ITEM_TEA_RASPBERRY) {
            found_tea = i;
            break;
        }
    }

    if (found_tea != -1) {
        int tea_id = p->inventory[found_tea];
        p->inventory[found_tea] = ITEM_EMPTY; 

        if (tea_id == ITEM_TEA_BLACK) {
            p->stamina += 50;
            if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;
            sprintf(g_last_event, "Вы выпили черный чай. +50 стамины.");
        } else if (tea_id == ITEM_TEA_GREEN) {
            p->stamina += 20;
            if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;
            p->hp += 20;
            if (p->hp > p->max_hp) p->hp = p->max_hp;
            sprintf(g_last_event, "Вы выпили зеленый чай. +20 стамины, +20 HP.");
        } else if (tea_id == ITEM_TEA_RASPBERRY) {
            p->hp += 50;
            if (p->hp > p->max_hp) p->hp = p->max_hp;
            sprintf(g_last_event, "Вы выпили малиновый чай. +50 HP.");
        }
    } else {
        sprintf(g_last_event, "В рюкзаке нет святого чая!");
    }
}

void action_move(Map *m, Player *p, char dir) {
    int nx = m->hero_x, ny = m->hero_y;
    if (dir == 'W') ny--;
    if (dir == 'S') ny++;
    if (dir == 'A') nx--;
    if (dir == 'D') nx++;

    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) {
        sprintf(g_last_event, "Там край карты.");
        return;
    }

    int interacted = entity_interact(m->entities, m->entity_count, p, nx, ny, m->drops, &m->drop_count, m->map, m->width, m->height);

    if (!interacted) {
        int moved = map_move(m, dir);
        if (moved) {
            p->stamina--;
            if (p->stamina < 0) p->stamina = 0;
            
            if (m->map[m->hero_y][m->hero_x] == TOWER_STAIR_UP) {
                sprintf(g_last_event, "Вы поднялись на этаж %d!", m->floor + 1);
                int reward = 10 * m->floor;
                p->gold += reward;
                p->stamina -= 5;
                if (p->stamina < 0) p->stamina = 0;
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
}

void adventure_loop(Player *player) {
    Map *m = map_create(1);
    if (!m) return;
    
    int running = 1;
    int start_gold = player->gold;
    
    while (running) {
        map_print(m, player);
        printf("Время: %02d:00 | Последнее событие: %s\n", g_hour, g_last_event);
        printf("Управление: W/A/S/D — ход/атака, I — инвентарь, E — подобрать, U — выпить чай, Q — выход\n");
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

        if (dir == 'I') { player_inventory_menu(player); continue; }
        if (dir == 'E') { action_pickup(m, player); continue; }
        if (dir == 'U') { action_drink_tea(player); continue; }

        if (dir != 'W' && dir != 'A' && dir != 'S' && dir != 'D') {
            sprintf(g_last_event, "Неизвестная команда.");
            continue;
        }

        action_move(m, player, dir);
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
        printf("[5] Инвентарь\n");
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
            case 5:
                if (!player) { printf("Сначала создайте персонажа!\n"); break; }
                player_inventory_menu(player);
                break;
            case 10:
                if (!player) { printf("Сначала создайте персонажа!\n"); break; }
                if (player->stamina <= 0) { printf("У вас нет сил идти в приключение!\n"); break; }
                if (g_hour == 2) { printf("Сейчас 02:00 ночи, фермер должен спать!\n"); break; }
                adventure_loop(player);
                break;
            default: printf("Нет такого пункта.\n");
        }
    }
    return 0;
}