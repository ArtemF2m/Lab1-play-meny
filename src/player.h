#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

#define INV_SIZE 10

typedef enum {
    SLOT_HEAD,
    SLOT_BODY,
    SLOT_FEET,
    SLOT_COUNT
} EquipSlot;

typedef enum {
    TYPE_NONE,
    TYPE_HEAD,
    TYPE_BODY,
    TYPE_FEET
} ItemType;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    ItemType type;
    int hp_bonus;
    int stamina_bonus;
} Item;

typedef struct {
    Item items[SLOT_COUNT];
    int is_equipped[SLOT_COUNT];
} Equipment;

typedef struct {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x, y;
    int attack;  // НОВОЕ
    int defense; // НОВОЕ
    int inventory[INV_SIZE];
    Equipment equip;
} Player;

Player* player_create(void);
void    player_free(Player **p);
void    player_print_card(const Player *p);
void    player_update_stats(Player *p);
void    player_equip_menu(Player *p);

#endif