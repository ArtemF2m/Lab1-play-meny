#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ID предметов
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
#define ITEM_LEATHER 11
#define ITEM_HELMET  12 
#define ITEM_CHEST   13
#define ITEM_BOOTS   14

#define MAX_ITEMS    15
#define MAX_NAME_LEN 32

// Символы карты
#define TOWER_WALL       '*' // Дерево
#define TOWER_FLOOR      '.'
#define TOWER_MIRROR     '|'
#define TOWER_MIRROR_H   '-'
#define TOWER_STAIR_UP   'H'//лестнеца 
#define TOWER_STAIR_DOWN 'D'//выход
#define TOWER_SWITCH     'X'//рычаг 
#define TOWER_HERO       '@'//герой собака
#define TOWER_MIRROR_HERO 'M'//монстр
#define TOWER_CRITTER 'r'

// Символы выпавших предметов
#define DROP_LOG   'L'
#define DROP_STONE 'S'
#define DROP_GOLD  'G'

extern char g_item_names[MAX_ITEMS][MAX_NAME_LEN];
extern char g_last_event[256];

int  read_int(int *value);
void read_line(char *buf, int size);
void load_item_names(void);

#endif