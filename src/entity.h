#ifndef ENTITY_H
#define ENTITY_H

#include "player.h"

#define MAX_ENTITIES 200
#define MAX_DROPS 100

typedef enum {
    ENT_NONE,
    ENT_TREE,
    ENT_ROCK,     // Валун (R)
    ENT_STONE,    // Обычный камень (#)
    ENT_HEAL,
    ENT_MONSTER,
    ENT_CRITTER,   // Пугливая дичь (r)
    ENT_GRASS    //травка 
} EntityType;

typedef struct {
    EntityType type;
    char symbol;
    int x, y;
    int hp, max_hp;
    int attack;
    int drop_item_id;
    int is_alive;
} Entity;

typedef struct {
    int x, y;
    int item_id;
    int is_alive;
} Drop;

int entity_generate(Entity **entities, char **map, int width, int height, int floor);
void entity_free(Entity **entities);

void spawn_drop(Drop *drops, int *drop_count, char **map, int width, int height, int x, int y, int item_id, int amount, Entity *entities, int entity_count);

int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y, Drop *drops, int *drop_count, char **map, int width, int height);

void entity_update_critters(Entity *entities, int count, char **map, int width, int height, int hero_x, int hero_y);

#endif