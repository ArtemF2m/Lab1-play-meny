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
            EntityType type = ENT_NONE;
            char sym = 0;
            
            // T - дерево, R - валун, а старые '*' и '#' просто стены
            if (map[y][x] == TOWER_WALL) { type = ENT_TREE; sym = 'T'; }
            else if (map[y][x] == 'R') { type = ENT_ROCK; sym = 'R'; }
            else if (rand() % 100 < 5) { type = ENT_HEAL; sym = '+'; }
            else if (rand() % 100 < 10) { type = ENT_MONSTER; sym = 'M'; }

            if (type != ENT_NONE) {
                Entity e;
                e.type = type;
                e.symbol = sym;
                e.x = x;
                e.y = y;
                e.is_alive = 1;
                float mult = 1.0f + (floor - 1) * 0.2f; 
                
                if (type == ENT_TREE) {
                    e.hp = e.max_hp = (int)(5 * mult);
                    e.drop_item_id = ITEM_WOOD;
                    e.attack = 0;
                } else if (type == ENT_ROCK) {
                    e.hp = e.max_hp = 8 * floor; // Твердый валун
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
                }
                
                (*entities)[count] = e;
                count++;
                
                // Очищаем карту от старых символов
                if (type != ENT_HEAL && type != ENT_MONSTER) map[y][x] = TOWER_FLOOR;
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

void spawn_drop(Drop *drops, int *drop_count, int x, int y, int item_id, int amount) {
    for (int a = 0; a < amount; a++) {
        if (*drop_count >= MAX_DROPS) return;
        
        int dx[] = {0, 0, -1, 1, -1, 1, -1, 1};
        int dy[] = {-1, 1, 0, 0, -1, -1, 1, 1};
        int placed = 0;
        
        for (int i = 0; i < 8 && !placed; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            int occupied = 0;
            for (int j = 0; j < *drop_count; j++) {
                if (drops[j].x == nx && drops[j].y == ny) { occupied = 1; break; }
            }
            if (!occupied) {
                drops[*drop_count].x = nx;
                drops[*drop_count].y = ny;
                drops[*drop_count].item_id = item_id;
                drops[*drop_count].is_alive = 1;
                (*drop_count)++;
                placed = 1;
            }
        }
    }
}

int find_entity(Entity *entities, int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].x == x && entities[i].y == y) return i;
    }
    return -1;
}

int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y, Drop *drops, int *drop_count) {
    int idx = find_entity(entities, count, target_x, target_y);
    if (idx == -1) return 0;

    Entity *e = &entities[idx];
    p->stamina -= 2;
    if (p->stamina < 0) p->stamina = 0;

    if (e->type == ENT_TREE || e->type == ENT_ROCK) {
        e->hp -= p->attack;
        if (e->hp <= 0) {
            e->is_alive = 0;
            if (e->type == ENT_TREE) {
                spawn_drop(drops, drop_count, e->x, e->y, ITEM_WOOD, 1);
                sprintf(g_last_event, "Хрясь! Дерево упало. Подберите бревно (L).");
            } else {
                int amount = 2 + rand() % 2;
                spawn_drop(drops, drop_count, e->x, e->y, ITEM_STONE, amount);
                sprintf(g_last_event, "БАБАХ! Валун разлетелся на %d камня(ей).", amount);
            }
        } else {
            sprintf(g_last_event, "Бам! %s трещит. Осталось прочности: %d", 
                    (e->type == ENT_TREE ? "Дерево" : "Валун"), e->hp);
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
            spawn_drop(drops, drop_count, e->x, e->y, ITEM_GOLD, gold_amount);
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