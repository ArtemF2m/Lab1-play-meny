#ifndef ENTITY_H
#define ENTITY_H

#include "player.h"

#define MAX_ENTITIES 50
#define MAX_DROPS 50 // Максимальное количество предметов на земле

typedef enum {
    ENT_NONE,
    ENT_TREE,
    ENT_ROCK,
    ENT_HEAL,
    ENT_MONSTER
} EntityType;

// Структура для живых объектов (деревья, монстры)
typedef struct {
    EntityType type;
    char symbol;
    int x, y;
    int hp, max_hp;
    int attack;
    int drop_item_id;
    int is_alive;
} Entity;

// Структура для предметов, лежащих на земле (бревна, камни, золото)
typedef struct {
    int x, y;
    int item_id;
    int is_alive;
} Drop;

int entity_generate(Entity **entities, char **map, int width, int height, int floor);
void entity_free(Entity **entities);

// Функция для спавна дропа (вызывается из entity_interact)
void spawn_drop(Drop *drops, int *drop_count, int x, int y, int item_id, int amount);

// Взаимодействие (теперь принимает массив дропа и его счетчик)
int entity_interact(Entity *entities, int count, Player *p, int target_x, int target_y, Drop *drops, int *drop_count);

#endif