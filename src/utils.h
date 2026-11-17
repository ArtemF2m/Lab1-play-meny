#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOWER_WALL       '*'
#define TOWER_FLOOR      '.'
#define TOWER_ROCK       '#'
#define TOWER_MIRROR     '|'
#define TOWER_MIRROR_H   '-'
#define TOWER_STAIR_UP   'H'
#define TOWER_STAIR_DOWN 'D'
#define TOWER_SWITCH     'X'
#define TOWER_HERO       '@'
#define TOWER_MIRROR_HERO 'M'
#define ITEM_EMPTY   0
#define ITEM_WOOD    1
#define ITEM_STONE   2
#define ITEM_SEEDS   3
#define ITEM_IRON    4
#define ITEM_WATER   5
#define ITEM_FOOD    6
#define ITEM_ROPE    7
#define ITEM_TORCH   8
#define ITEM_GOLD    9
#define ITEM_PICKAXE 10
#define ITEM_LEATHER 11 // НОВОЕ (для дропа с монстров)

#define MAX_ITEMS    12 // Увеличили из-за нового предмета
#define MAX_NAME_LEN 32

extern char g_item_names[MAX_ITEMS][MAX_NAME_LEN];
extern char g_last_event[256];

int  read_int(int *value);
void read_line(char *buf, int size);
void load_item_names(void);

#endif