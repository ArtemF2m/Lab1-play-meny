#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MAP_MIN_SIZE      15
#define MAP_MAX_SIZE      25

#define TOWER_WALL       '*'
#define TOWER_FLOOR      '.'
#define TOWER_HERO       '@'
#define TOWER_MIRROR_HERO 'M'
#define TOWER_STAIR_UP   'H'
#define TOWER_STAIR_DOWN 'D'
#define TOWER_SWITCH     'X'
#define TOWER_MIRROR     '|'
#define TOWER_MIRROR_H   '-'
#define TOWER_ROCK       '#'
#define TOWER_STONE_BROKEN 'S'

#define TOWER_OBSTACLE_MIN 10
#define TOWER_OBSTACLE_MAX 25

typedef struct {
    char **map;
    int    height;
    int    width;
    int    hero_x, hero_y;
    int    mirror_hero_x, mirror_hero_y;
    int    split_vertical;
    int    split_pos;
    int    mirror_broken;
    int    floor;
} Map;

Map* map_create(int floor);
void map_free(Map **m);
void map_print(const Map *m, const Player *p);
int  map_move(Map *m, char dir);

#endif