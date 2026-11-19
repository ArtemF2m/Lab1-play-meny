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

typedef struct {
    int item_id[SLOT_COUNT]; // МАССИВ ID (как в Minecraft) - ИСПРАВЛЕНО
} Equipment;

typedef struct {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x, y;
    int attack;
    int defense;
    int inventory[INV_SIZE];
    Equipment equip;
} Player;

Player* player_create(void);
void    player_free(Player **p);
void    player_print_card(const Player *p);
void    player_update_stats(Player *p);
void    player_equip_menu(Player *p);
void    player_get_item_stats(int item_id, int *hp_bonus, int *stamina_bonus, EquipSlot *slot);

#endif