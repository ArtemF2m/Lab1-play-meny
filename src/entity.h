#ifndef ENTITY_H
#define ENTITY_H

#include "player.h"

#define MAX_ENTITIES 50

typedef enum {
    ENT_NONE,
    ENT_TREE,
    ENT_ROCK,
    ENT_HEAL,
    ENT_MONSTER
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

int entity_generate(Entity **entities, char **map, int width, int height, int floor);
void entity_free(Entity **entities);
int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y);

#endif