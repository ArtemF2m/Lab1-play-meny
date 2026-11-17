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
            
            if (map[y][x] == TOWER_WALL) { type = ENT_TREE; sym = 'T'; }
            else if (map[y][x] == TOWER_ROCK) { type = ENT_ROCK; sym = 'R'; }
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
                    e.hp = e.max_hp = (int)(8 * mult);
                    e.drop_item_id = ITEM_STONE;
                    e.attack = 0;
                } else if (type == ENT_MONSTER) {
                    e.hp = e.max_hp = (int)(20 * mult);
                    e.attack = (int)(3 * mult);
                    e.drop_item_id = ITEM_LEATHER;
                } else if (type == ENT_HEAL) {
                    e.hp = e.max_hp = 1;
                    e.drop_item_id = ITEM_FOOD;
                    e.attack = 0;
                }
                
                (*entities)[count] = e;
                count++;
                
                // Очищаем карту от старых символов, чтобы рисовать только сущности
                if (type != ENT_HEAL && type != ENT_MONSTER) {
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

int find_entity(Entity *entities, int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].x == x && entities[i].y == y) {
            return i;
        }
    }
    return -1;
}

int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y) {
    int idx = find_entity(entities, count, target_x, target_y);
    if (idx == -1) return 0;

    Entity *e = &entities[idx];
    p->stamina -= 2;
    if (p->stamina < 0) p->stamina = 0;

    if (e->type == ENT_TREE || e->type == ENT_ROCK) {
        e->hp -= p->attack;
        if (e->hp <= 0) {
            e->is_alive = 0;
            int slot = -1;
            for (int i = 0; i < INV_SIZE; i++) {
                if (p->inventory[i] == ITEM_EMPTY) { slot = i; break; }
            }
            if (slot != -1) {
                p->inventory[slot] = e->drop_item_id;
                sprintf(g_last_event, "Вы сломали %s и получили: %s!", 
                        (e->type == ENT_TREE ? "дерево" : "камень"), g_item_names[e->drop_item_id]);
            } else {
                sprintf(g_last_event, "Рюкзак полон! Ресурс пропал.");
            }
        } else {
            sprintf(g_last_event, "Вы ударили %s. Прочность: %d", 
                    (e->type == ENT_TREE ? "дерево" : "камень"), e->hp);
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
            int gold_reward = 10 + rand() % 15;
            p->gold += gold_reward;
            sprintf(g_last_event, "БОНЬК! Монстр повержен. Вы получили %d золота.", gold_reward);
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