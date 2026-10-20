// Лабораторная работа №3
// Вариант 8: Оценка рюкзака у скупщика
#include <stdio.h>
#include <string.h>
#define INVENTORY_SIZE   10
#define HOURS_IN_DAY     24
#define START_DAY        1
#define START_HOUR       8
#define MAX_ITEMS        10
#define MAX_NAME_LEN     32
#define MAX_FARMER_NAME  32
#define MAX_PRICE_ITEMS  50
#define MAX_PRICE_LEN    32
// ID предметов
#define ITEM_EMPTY  0
#define ITEM_WOOD   1
#define ITEM_STONE  2
#define ITEM_SEEDS  3
#define ITEM_IRON   4
#define ITEM_WATER  5
#define ITEM_FOOD   6
#define ITEM_ROPE   7
#define ITEM_TORCH  8
#define ITEM_GOLD   9
// Глобальное состояние
int current_day  = START_DAY;
int current_hour = START_HOUR;
int inventory[INVENTORY_SIZE] = {0};
char farmer_name[MAX_FARMER_NAME] = "Фермер";
char item_names[MAX_ITEMS][MAX_NAME_LEN];
// Протипы
int  readInt(int *value);
void readLine(char *buf, int size);
void loadItemNames(void);
void askFarmerName(void);
void showMenu(void);
void showTime(void);
void workHours(void);
void showInventory(void);
void putItem(void);
void dropItem(void);
void findItemByName(void);
void writeDiary(void);
void variantTask(void);
//меню
int main(void) {
    loadItemNames();
    askFarmerName();

    int choice;
    while (1) {
        showMenu();
        if (!readInt(&choice)) {
            printf("Ошибка: нужно ввести число.\n");
            continue;
        }
        switch (choice) {
            case 0:
                printf("Выход из игры.\n");
                return 0;
            case 1: showTime();        break;
            case 2: workHours();       break;
            case 3: showInventory();   break;
            case 4: putItem();         break;
            case 5: dropItem();        break;
            case 6: findItemByName();  break;
            case 7: writeDiary();      break;
            case 8: variantTask();     break;
            default:
                printf("Нет такого пункта меню.\n");
        }
    }
    return 0;
}
//Ввод
int readInt(int *value) {
    if (scanf("%d", value) != 1) {
        while (getchar() != '\n');
        return 0;
    }
    return 1;
}

void readLine(char *buf, int size) {
    if (fgets(buf, size, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
    }
}
//Загрузка каталога
void loadItemNames(void) {
    const char *defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена", "Железо",
        "Вода", "Еда", "Веревка", "Факел", "Золото"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(item_names[i], defaults[i], MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
    }
    FILE *f = fopen("items.txt", "r");
    if (!f) {
        printf("Внимание: items.txt не найден, использую стандартные названия.\n");
        return;
    }

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        int id;
        char name[MAX_NAME_LEN];
        if (sscanf(line, "%d %31s", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) {
                strncpy(item_names[id], name, MAX_NAME_LEN - 1);
                item_names[id][MAX_NAME_LEN - 1] = '\0';
            }
        }
    }
    fclose(f);
    printf("Каталог предметов загружен из items.txt\n");
}
//Имя весёлого фермера
void askFarmerName(void) {
    printf("Введите имя весёлого фермера (до %d символов): ", MAX_FARMER_NAME - 1);
    readLine(farmer_name, sizeof(farmer_name));
    if (strlen(farmer_name) == 0) {
        strncpy(farmer_name, "Фермер", MAX_FARMER_NAME - 1);
        farmer_name[MAX_FARMER_NAME - 1] = '\0';
    }
    printf("Привет, %s!\n", farmer_name);
}
//Меню
void showMenu(void) {
    printf("\n===== Фермер: %s | День %d, %02d:00 =====\n",
           farmer_name, current_day, current_hour);
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время (Поработать)\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Поиск предмета по названию\n");
    printf("[7] Записать в дневник\n");
	printf("[8] Оценка рюкзака у скупщика\n");
    printf("Выбор: ");
}
//Время
void showTime(void) {
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
}
void workHours(void) {
    int hours;
    printf("Сколько часов работаем? ");
    if (!readInt(&hours)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (hours < 0) {
        printf("Нельзя работать отрицательное время.\n");
        return;
    }
    current_hour += hours;
    while (current_hour >= HOURS_IN_DAY) {
        current_hour -= HOURS_IN_DAY;
        current_day++;
    }
    printf("Прошло %d ч. ", hours);
    showTime();
}
//Инвентарь
void showInventory(void) {
    printf("\n--- Рюкзак ---\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: [%d] - %s\n",
               i, inventory[i], item_names[inventory[i]]);
    }
}
void putItem(void) {
    int slot, id;
    printf("Индекс слота (0..%d): ", INVENTORY_SIZE - 1);
    if (!readInt(&slot)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: слот вне диапазона.\n");
        return;
    }
    printf("ID предмета (0..%d): ", MAX_ITEMS - 1);
    if (!readInt(&id)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (id < 0 || id >= MAX_ITEMS) {
        printf("Ошибка: такого ID нет.\n");
        return;
    }
    inventory[slot] = id;
    printf("В слот %d положено: %s\n", slot, item_names[id]);
}
void dropItem(void) {
    int slot;
    printf("Индекс слота (0..%d): ", INVENTORY_SIZE - 1);
    if (!readInt(&slot)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: слот вне диапазона.\n");
        return;
    }
    inventory[slot] = ITEM_EMPTY;
    printf("Слот %d очищен.\n", slot);
}
//Поиск по названию
void findItemByName(void) {
    char query[MAX_NAME_LEN];
    printf("Введите название предмета: ");
    if (scanf("%31s", query) != 1) {
        while (getchar() != '\n');
        printf("Ошибка ввода.\n");
        return;
    }

    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], query) == 0) {
            found_id = i;
            break;
        }
    }
    if (found_id == -1) {
        printf("Такого предмета нет в каталоге.\n");
        return;
    }

    printf("ID = %d. Ищу в рюкзаке...\n", found_id);
    int count = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == found_id) {
            printf("  Найден в слоте %d\n", i);
            count++;
        }
    }
    if (count == 0) printf("В рюкзаке такого предмета нет.\n");
}
//девник
void writeDiary(void) {
    FILE *f = fopen("diary.txt", "a");
    if (!f) {
        printf("Не удалось открыть diary.txt для записи.\n");
        return;
    }
    fprintf(f, "===== Запись =====\n");
    fprintf(f, "Фермер: %s\n", farmer_name);
    fprintf(f, "День %d, %02d:00\n", current_day, current_hour);
    fprintf(f, "Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        fprintf(f, "  Слот %d: %s\n", i, item_names[inventory[i]]);
    }
    fprintf(f, "\n");
    fclose(f);
    printf("Записано в diary.txt\n");
}
// Оценка рюкзака у скупщика: читаем прайс из input.txt,
// считаем стоимость инвентаря, пишем отчёт в output.txt
void variantTask(void) {
    char price_names[MAX_PRICE_ITEMS][MAX_PRICE_LEN];
    int  price_values[MAX_PRICE_ITEMS];
    int  price_count = 0;
    FILE *in = fopen("input.txt", "r");
    if (!in) {
        printf("Ошибка: не удалось открыть input.txt\n");
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), in) && price_count < MAX_PRICE_ITEMS) {
        char name[MAX_PRICE_LEN];
        int  price;
        // формат: "Название Цена" (например "Дерево 15")
        if (sscanf(line, "%31s %d", name, &price) == 2) {
            strncpy(price_names[price_count], name, MAX_PRICE_LEN - 1);
            price_names[price_count][MAX_PRICE_LEN - 1] = '\0';
            price_values[price_count] = price;
            price_count++;
        }
    }
    fclose(in);

    if (price_count == 0) {
        printf("Прейскурант пуст или повреждён.\n");
        return;
    }
    //Считаем слоты
    int slot_price[INVENTORY_SIZE];
    int total = 0;
    int best_slot = -1;
    int best_price = -1;

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        slot_price[i] = 0;
        if (inventory[i] == ITEM_EMPTY) continue;

        const char *item_name = item_names[inventory[i]];

        for (int j = 0; j < price_count; j++) {
            if (strcmp(item_name, price_names[j]) == 0) {
                slot_price[i] = price_values[j];
                break;
            }
        }
        total += slot_price[i];

        if (slot_price[i] > best_price) {
            best_price = slot_price[i];
            best_slot  = i;
        }
    }
    //Вывод на экран
    printf("\n--- Оценочная ведомость (скупщик) ---\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: %-15s — %d монет\n",
               i, item_names[inventory[i]], slot_price[i]);
    }
    if (best_slot != -1 && best_price > 0) {
        printf("Самый дорогой предмет: %s (слот %d) — %d монет\n",
               item_names[inventory[best_slot]], best_slot, best_price);
    } else {
        printf("Самый дорогой предмет: нет (все слоты пусты или без цены)\n");
    }
    printf("Общая стоимость рюкзака: %d монет\n", total);

    //Запись в txt
    FILE *out = fopen("output.txt", "w");
    if (!out) {
        printf("Ошибка: не удалось открыть output.txt для записи.\n");
        return;
    }

    fprintf(out, "Оценочная ведомость скупщика\n");
    fprintf(out, "Фермер: %s\n", farmer_name);
    fprintf(out, "День %d, %02d:00\n\n", current_day, current_hour);

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        fprintf(out, "Слот %d: %-15s — %d монет\n",
                i, item_names[inventory[i]], slot_price[i]);
    }
    if (best_slot != -1 && best_price > 0) {
        fprintf(out, "Самый дорогой предмет: %s (слот %d) — %d монет\n",
                item_names[inventory[best_slot]], best_slot, best_price);
    } else {
        fprintf(out, "Самый дорогой предмет: нет\n");
    }
    fprintf(out, "Общая стоимость рюкзака: %d монет\n", total);

    fclose(out);
    printf("Отчёт записан в output.txt\n");
}
