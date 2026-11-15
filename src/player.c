#include "player.h"
#include <stdio.h>

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
    p->x = 0; p->y = 0;

    for (int i = 0; i < INV_SIZE; i++) p->inventory[i] = ITEM_EMPTY;
    p->inventory[0] = ITEM_PICKAXE; 

    for (int i = 0; i < SLOT_COUNT; i++) {
        p->equip.is_equipped[i] = 0;
        p->equip.items[i].hp_bonus = 0;
        p->equip.items[i].stamina_bonus = 0;
        strcpy(p->equip.items[i].name, "Нет");
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
        if (p->equip.is_equipped[i]) {
            p->max_hp += p->equip.items[i].hp_bonus;
            p->max_stamina += p->equip.items[i].stamina_bonus;
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
        printf("[1] Надеть Шлем (Голова, +20 HP)\n");
        printf("[2] Надеть Броню (Тело, +30 HP, +10 Стамина)\n");
        printf("[3] Надеть Сапоги (Обувь, +10 Стамина)\n");
        printf("[4] Снять всё\n");
        printf("[0] Назад\n");
        printf("Выбор: ");

        if (!read_int(&choice)) {
            printf("Ошибка ввода.\n");
            continue;
        }

        switch (choice) {
            case 0: return;
            case 1:
                p->equip.is_equipped[SLOT_HEAD] = 1;
                strcpy(p->equip.items[SLOT_HEAD].name, "Шлем");
                p->equip.items[SLOT_HEAD].hp_bonus = 20;
                p->equip.items[SLOT_HEAD].stamina_bonus = 0;
                player_update_stats(p);
                break;
            case 2:
                p->equip.is_equipped[SLOT_BODY] = 1;
                strcpy(p->equip.items[SLOT_BODY].name, "Броня");
                p->equip.items[SLOT_BODY].hp_bonus = 30;
                p->equip.items[SLOT_BODY].stamina_bonus = 10;
                player_update_stats(p);
                break;
            case 3:
                p->equip.is_equipped[SLOT_FEET] = 1;
                strcpy(p->equip.items[SLOT_FEET].name, "Сапоги");
                p->equip.items[SLOT_FEET].hp_bonus = 0;
                p->equip.items[SLOT_FEET].stamina_bonus = 10;
                player_update_stats(p);
                break;
            case 4:
                for (int i = 0; i < SLOT_COUNT; i++) {
                    p->equip.is_equipped[i] = 0;
                    p->equip.items[i].hp_bonus = 0;
                    p->equip.items[i].stamina_bonus = 0;
                    strcpy(p->equip.items[i].name, "Нет");
                }
                player_update_stats(p);
                break;
            default: printf("Нет такого пункта.\n");
        }
    }
}

void player_print_card(const Player *p) {
    printf("\n===== Карточка персонажа =====\n");
    printf("Имя: %s\n", p->name);
    printf("HP: %d/%d\n", p->hp, p->max_hp);
    printf("Выносливость: %d/%d\n", p->stamina, p->max_stamina);
    printf("Золото: %d\n", p->gold);
    printf("Инвентарь:\n");
    for (int i = 0; i < INV_SIZE; i++) {
        printf("  Слот %d: %s\n", i, g_item_names[p->inventory[i]]);
    }
    printf("Экипировка:\n");
    for (int i = 0; i < SLOT_COUNT; i++) {
        const char *slot_name = (i == SLOT_HEAD) ? "Голова" : (i == SLOT_BODY) ? "Тело" : "Обувь";
        printf("  %s: %s\n", slot_name, p->equip.is_equipped[i] ? p->equip.items[i].name : "Пусто");
    }
    printf("===============================\n");
}