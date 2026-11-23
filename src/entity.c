#include "entity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int entity_generate(Entity **entities, char **map, int width, int height, int floor) {
    *entities = (Entity*)malloc(MAX_ENTITIES * sizeof(Entity));
    if (!*entities) return 0;
    
    int count = 0;
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (count >= MAX_ENTITIES) break;
            
            // Защита от наложения сущностей
            int already_has = 0;
            for (int i = 0; i < count; i++) {
                if ((*entities)[i].x == x && (*entities)[i].y == y) {
                    already_has = 1;
                    break;
                }
            }
            if (already_has) continue;

            EntityType type = ENT_NONE;
            char sym = 0;
            
            if (map[y][x] == TOWER_WALL) { type = ENT_TREE; sym = 'T'; }
            else if (map[y][x] == 'R') { type = ENT_ROCK; sym = 'R'; }
            else if (map[y][x] == '#') { type = ENT_STONE; sym = '#'; }
            else if (rand() % 100 < 5) { type = ENT_HEAL; sym = '+'; }
            else if (rand() % 100 < 10) { type = ENT_MONSTER; sym = 'M'; }
            else if (rand() % 100 < 5) { type = ENT_CRITTER; sym = 'r'; }

            if (type != ENT_NONE) {
                Entity e;
                e.type = type;
                e.symbol = sym;
                e.x = x;
                e.y = y;
                e.is_alive = 1;
                float mult = 1.0f + (floor - 1) * 0.2f; 
                
                if (type == ENT_TREE) {
                    e.hp = e.max_hp = 50;
                    e.drop_item_id = ITEM_WOOD;
                    e.attack = 0;
                } else if (type == ENT_ROCK) {
                    e.hp = e.max_hp = 80;
                    e.drop_item_id = ITEM_STONE;
                    e.attack = 0;
                } else if (type == ENT_STONE) {
                    e.hp = e.max_hp = 35;
                    e.drop_item_id = ITEM_STONE;
                    e.attack = 0;
                } else if (type == ENT_MONSTER) {
                    e.hp = e.max_hp = (int)(20 * mult);
                    e.attack = (int)(3 * mult);
                    e.drop_item_id = ITEM_GOLD;
                } else if (type == ENT_HEAL) {
                    e.hp = e.max_hp = 1;
                    e.drop_item_id = ITEM_FOOD;
                    e.attack = 0;
                } else if (type == ENT_CRITTER) {
                    e.hp = e.max_hp = 1;
                    e.attack = 0;
                    e.drop_item_id = ITEM_GOLD;
                }
                
                (*entities)[count] = e;
                count++;
                
                // Очищаем карту от старых символов
                if (type != ENT_HEAL && type != ENT_MONSTER && type != ENT_CRITTER) {
                    map[y][x] = TOWER_FLOOR;
                }
            }
        }
    }
    return count;
}

void entity_free(Entity **entities) {
    if (*entities) {
        printf("Память массива сущностей по адресу %p освобождена.\n", (void*)*entities);
        free(*entities);
        *entities = NULL;
    }
}

void spawn_drop(Drop *drops, int *drop_count, char **map, int width, int height, int x, int y, int item_id, int amount, Entity *entities, int entity_count) {
    // Порядок выпадения: Центр, Вверх, Влево, Вниз, Вправо
    int dx[] = {0, 0, -1, 1, 1};
    int dy[] = {0, -1, 0, 1, 0};
    
    for (int a = 0; a < amount; a++) {
        if (*drop_count >= MAX_DROPS) return;
        int placed = 0;
        
        for (int i = 0; i < 5 && !placed; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
            
            char cell = map[ny][nx];
            if (cell == TOWER_WALL || cell == TOWER_MIRROR || cell == TOWER_MIRROR_H) continue;

            int occupied_by_entity = 0;
            for (int e = 0; e < entity_count; e++) {
                if (entities[e].is_alive && entities[e].x == nx && entities[e].y == ny) {
                    occupied_by_entity = 1;
                    break;
                }
            }
            if (occupied_by_entity) continue;

            int occupied_by_drop = 0;
            for (int j = 0; j < *drop_count; j++) {
                if (drops[j].x == nx && drops[j].y == ny) { occupied_by_drop = 1; break; }
            }
            if (occupied_by_drop) continue;
            
            drops[*drop_count].x = nx;
            drops[*drop_count].y = ny;
            drops[*drop_count].item_id = item_id;
            drops[*drop_count].is_alive = 1;
            (*drop_count)++;
            placed = 1;
        }
        
        if (!placed) break; 
    }
}

int find_entity(Entity *entities, int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].x == x && entities[i].y == y) return i;
    }
    return -1;
}

int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y, Drop *drops, int *drop_count, char **map, int width, int height) {
    int idx = find_entity(entities, count, target_x, target_y);
    if (idx == -1) return 0;

    Entity *e = &entities[idx];
    p->stamina -= 2;
    if (p->stamina < 0) p->stamina = 0;

    if (e->type == ENT_TREE || e->type == ENT_ROCK || e->type == ENT_STONE) {
        // ПРОВЕРКА НА КИРКУ
        int has_pickaxe = 0;
        for (int i = 0; i < INV_SIZE; i++) {
            if (p->inventory[i] == ITEM_PICKAXE) {
                has_pickaxe = 1;
                break;
            }
        }
        
        if (!has_pickaxe) {
            sprintf(g_last_event, "Вам нужна кирка, чтобы добыть этот ресурс!");
            return 1; 
        }

        e->hp -= p->attack;
        if (e->hp <= 0) {
            e->is_alive = 0;
            if (e->type == ENT_TREE) {
                spawn_drop(drops, drop_count, map, width, height, e->x, e->y, ITEM_WOOD, 1, entities, count);
                sprintf(g_last_event, "Хрясь! Дерево упало. Подберите бревно (L).");
            } else if (e->type == ENT_ROCK) {
                int amount = 2 + rand() % 2;
                spawn_drop(drops, drop_count, map, width, height, e->x, e->y, ITEM_STONE, amount, entities, count);
                sprintf(g_last_event, "БАБАХ! Валун разлетелся на %d камня(ей).", amount);
            } else if (e->type == ENT_STONE) {
                spawn_drop(drops, drop_count, map, width, height, e->x, e->y, ITEM_STONE, 1, entities, count);
                sprintf(g_last_event, "Бам! Камень разбился. Подберите камень (S).");
            }
        } else {
            const char *name = "камень";
            if (e->type == ENT_TREE) name = "Дерево";
            else if (e->type == ENT_ROCK) name = "Валун";
            else if (e->type == ENT_STONE) name = "Камень";
            sprintf(g_last_event, "Бам! %s трещит. Осталось: %d", name, e->hp);
        }
        return 1;
    }

    if (e->type == ENT_CRITTER) {
        e->is_alive = 0;
        int gold_reward = 10 + rand() % 20; // Чуть меньше золота, так как теперь даём чай
        p->gold += gold_reward;
        
        int slot = -1;
        for (int i = 0; i < INV_SIZE; i++) {
            if (p->inventory[i] == ITEM_EMPTY) { slot = i; break; }
        }
        
        if (slot != -1) {
            // Выбираем случайный чай (15, 16 или 17)
            int teas[] = {ITEM_TEA_BLACK, ITEM_TEA_GREEN, ITEM_TEA_RASPBERRY};
            int random_tea = teas[rand() % 3];
            
            p->inventory[slot] = random_tea;
            sprintf(g_last_event, "Вы поймали зверька! +%d золота и %s.", gold_reward, g_item_names[random_tea]);
        } else {
            sprintf(g_last_event, "Вы поймали зверька! +%d золота, но рюкзак полон.", gold_reward);
        }
        return 1;
    }
    
    if (e->type == ENT_HEAL) {
        e->is_alive = 0;
        p->hp += 20; if (p->hp > p->max_hp) p->hp = p->max_hp;
        p->stamina += 10; if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;
        sprintf(g_last_event, "Вы съели ягоды и восстановили силы!");
        return 1;
    }

    if (e->type == ENT_MONSTER) {
        int player_dmg = p->attack - (e->attack / 2);
        if (player_dmg < 1) player_dmg = 1;
        e->hp -= player_dmg;
        
        if (e->hp <= 0) {
            e->is_alive = 0;
            int gold_amount = 1 + rand() % 3; 
            spawn_drop(drops, drop_count, map, width, height, e->x, e->y, ITEM_GOLD, gold_amount, entities, count);
            sprintf(g_last_event, "БОНЬК! Монстр повержен. Выронил золото (G).");
        } else {
            int monster_dmg = e->attack - (p->defense / 2);
            if (monster_dmg < 1) monster_dmg = 1;
            p->hp -= monster_dmg;
            if (monster_dmg >= 5) {
                sprintf(g_last_event, "Ай! Монстр сильно укусил (-%d HP).", monster_dmg);
            } else {
                sprintf(g_last_event, "Ой! Монстр чехнул на вас (-%d HP).", monster_dmg);
            }
        }
        return 1;
    }
    return 1;
}

void entity_update_critters(Entity *entities, int count, char **map, int width, int height, int hero_x, int hero_y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].type == ENT_CRITTER) {
            int dist = abs(entities[i].x - hero_x) + abs(entities[i].y - hero_y);
            
            if (dist <= 3) { // Герой близко (в радиусе 3)
                int dx = 0, dy = 0;
                
                if (entities[i].x > hero_x) dx = 1;
                else if (entities[i].x < hero_x) dx = -1;
                
                if (entities[i].y > hero_y) dy = 1;
                else if (entities[i].y < hero_y) dy = -1;
                
                if (dx == 0 && dy == 0) { dx = (rand() % 2 == 0) ? 1 : -1; }
                
                int nx = entities[i].x + dx;
                int ny = entities[i].y + dy;
                
                int can_move = 1;
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) can_move = 0;
                else if (map[ny][nx] == TOWER_WALL || map[ny][nx] == TOWER_MIRROR || map[ny][nx] == TOWER_MIRROR_H) can_move = 0;
                
                for (int j = 0; j < count; j++) {
                    if (j != i && entities[j].is_alive && entities[j].x == nx && entities[j].y == ny) {
                        can_move = 0; break;
                    }
                }
                
                if (can_move) {
                    entities[i].x = nx;
                    entities[i].y = ny;
                }
            }
        }
    }
}