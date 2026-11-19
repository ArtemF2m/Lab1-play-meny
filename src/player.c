#include "player.h"
#include <stdio.h>

void player_get_item_stats(int item_id, int *hp_bonus, int *stamina_bonus, EquipSlot *slot) {
    *hp_bonus = 0;
    *stamina_bonus = 0;
    *slot = SLOT_HEAD;

    if (item_id == ITEM_HELMET) {
        *hp_bonus = 20;
        *slot = SLOT_HEAD;
    } else if (item_id == ITEM_CHEST) {
        *hp_bonus = 30;
        *stamina_bonus = 10;
        *slot = SLOT_BODY;
    } else if (item_id == ITEM_BOOTS) {
        *stamina_bonus = 10;
        *slot = SLOT_FEET;
    }
}

Player* player_create(void) {
    Player *p = (Player*)malloc(sizeof(Player));
    if (!p) return NULL;

    printf("Введите имя персонажа: ");
    read_line(p->name, MAX_NAME_LEN);
    if (strlen(p->name) == 0) strcpy(p->name, "Безымянный");

    p->max_hp = 100;
    p->hp = 100;
    p->max_stamina = 50;
    p->stamina = 50;
    p->gold = 50;
    p->attack = 5;
    p->defense = 2;

    for (int i = 0; i < INV_SIZE; i++) p->inventory[i] = ITEM_EMPTY;
    p->inventory[0] = ITEM_PICKAXE; 

    for (int i = 0; i < SLOT_COUNT; i++) {
        p->equip.item_id[i] = ITEM_EMPTY;
    }

    printf("Создан персонаж: %s (Адрес: %p)\n", p->name, (void*)p);
    return p;
}

void player_free(Player **p) {
    if (p && *p) {
        printf("Память структуры по адресу %p освобождена.\n", (void*)*p);
        free(*p);
        *p = NULL;
    }
}

void player_update_stats(Player *p) {
    p->max_hp = 100;
    p->max_stamina = 50;

    for (int i = 0; i < SLOT_COUNT; i++) {
        if (p->equip.item_id[i] != ITEM_EMPTY) {
            int hp_b, st_b;
            EquipSlot dummy;
            player_get_item_stats(p->equip.item_id[i], &hp_b, &st_b, &dummy);
            p->max_hp += hp_b;
            p->max_stamina += st_b;
        }
    }

    if (p->hp > p->max_hp) p->hp = p->max_hp;
    if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;

    printf("Экипировка обновлена. HP: %d/%d, Стамина: %d/%d\n",
           p->hp, p->max_hp, p->stamina, p->max_stamina);
}

void player_equip_menu(Player *p) {
    int choice;
    while (1) {
        printf("\n--- Управление снаряжением ---\n");
        printf("[1] Надеть Шлем (ID: %d)\n", ITEM_HELMET);
        printf("[2] Надеть Броню (ID: %d)\n", ITEM_CHEST);
        printf("[3] Надеть Сапоги (ID: %d)\n", ITEM_BOOTS);
        printf("[4] Снять всё\n");
        printf("[0] Назад\n");
        printf("Выбор: ");

        if (!read_int(&choice)) continue;

        if (choice == 0) return;
        
        if (choice >= 1 && choice <= 3) {
            int item_id = (choice == 1) ? ITEM_HELMET : (choice == 2) ? ITEM_CHEST : ITEM_BOOTS;
            
            int hp_b, st_b;
            EquipSlot slot;
            player_get_item_stats(item_id, &hp_b, &st_b, &slot);
            
            p->equip.item_id[slot] = item_id;
            printf("Вы надели: %s\n", g_item_names[item_id]);
            player_update_stats(p);
        } 
        else if (choice == 4) {
            for (int i = 0; i < SLOT_COUNT; i++) {
                p->equip.item_id[i] = ITEM_EMPTY;
            }
            player_update_stats(p);
        }
    }
}

void player_print_card(const Player *p) {
    printf("\n===== Карточка персонажа =====\n");
    printf("Имя: %s\n", p->name);
    printf("HP: %d/%d | Стамина: %d/%d\n", p->hp, p->max_hp, p->stamina, p->max_stamina);
    printf("Атака: %d | Защита: %d\n", p->attack, p->defense);
    printf("Золото: %d\n", p->gold);
    printf("Инвентарь:\n");
    for (int i = 0; i < INV_SIZE; i++) {
        printf("  Слот %d: %s\n", i, g_item_names[p->inventory[i]]);
    }
    printf("Экипировка:\n");
    const char *slot_names[] = {"Голова", "Тело", "Обувь"};
    for (int i = 0; i < SLOT_COUNT; i++) {
        printf("  %s: %s\n", slot_names[i], p->equip.item_id[i] == ITEM_EMPTY ? "Пусто" : g_item_names[p->equip.item_id[i]]);
    }
    printf("===============================\n");
}