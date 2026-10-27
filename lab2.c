// Лабораторная работа №3
// Вариант 8: Оценка рюкзака у скупщика
// Лабораторная работа №4
// Вариант:8 Зеркальная комната (Башня)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

// ===== ЛР4: Константы карты =====
#define MAP_MIN_SIZE      5
#define MAP_MAX_SIZE      30

#define TOWER_WALL       '#'
#define TOWER_FLOOR      '.'
#define TOWER_HERO       '@'
#define TOWER_STAIR_UP   'U'   // лестница вверх (новый этаж)
#define TOWER_STAIR_DOWN 'D'   // лестница вниз (выход домой)
#define TOWER_SWITCH     'X'   // переключатель (убирает стену)
#define TOWER_MIRROR     '|'   // разделительная стена (вертикальная)
#define TOWER_MIRROR_H   '-'   // разделительная стена (горизонтальная)

#define TOWER_OBSTACLE_MIN 10
#define TOWER_OBSTACLE_MAX 25

// Глобальное состояние
int current_day  = START_DAY;
int current_hour = START_HOUR;
int inventory[INVENTORY_SIZE] = {0};
char farmer_name[MAX_FARMER_NAME] = "Фермер";
char item_names[MAX_ITEMS][MAX_NAME_LEN];

// ===== ЛР4: Структура башни =====
typedef struct {
    char **map;
    int    height;
    int    width;
    int    hero_x, hero_y;
    int    split_vertical;   // 1 — стена вертикальная, 0 — горизонтальная
    int    split_pos;        // координата разделительной стены
    int    mirror_broken;    // 1 — переключатель уже нажат
    int    floor;            // номер этажа
} TowerState;

// Прототипы (ЛР2-3)
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

// Прототипы (ЛР4)
char **allocateMap(int height, int width);
void   freeMap(char **map, int height);
int    towerGenerate(TowerState *t);
void   towerFree(TowerState *t);
void   towerPrint(const TowerState *t);
int    towerTryMove(TowerState *t, char dir);
int    towerCheckOnStairDown(const TowerState *t);
int    towerCheckOnStairUp(const TowerState *t);
void   towerAdventure(void);

// ===== main =====
int main(void) {
    srand((unsigned)time(NULL));

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
            case 10: towerAdventure(); break;
            default:
                printf("Нет такого пункта меню.\n");
        }
    }
    return 0;
}

// ===== Ввод =====
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

// ===== Загрузка каталога =====
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

// ===== Имя фермера =====
void askFarmerName(void) {
    printf("Введите имя весёлого фермера (до %d символов): ", MAX_FARMER_NAME - 1);
    readLine(farmer_name, sizeof(farmer_name));
    if (strlen(farmer_name) == 0) {
        strncpy(farmer_name, "Фермер", MAX_FARMER_NAME - 1);
        farmer_name[MAX_FARMER_NAME - 1] = '\0';
    }
    printf("Привет, %s!\n", farmer_name);
}

// ===== Меню =====
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
    printf("[10] Войти в башню (Зеркальная комната)\n");
    printf("Выбор: ");
}

// ===== Время =====
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

// ===== Инвентарь =====
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

// ===== Поиск по названию =====
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

// ===== Дневник =====
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

// ===== Оценка рюкзака у скупщика (ЛР3, вариант 8) =====
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

// ============================================================
// =================== ЛР4: ЗЕРКАЛЬНАЯ КОМНАТА =================
// ============================================================

// Выделение двумерного динамического массива
char **allocateMap(int height, int width) {
    char **map = (char**)malloc(height * sizeof(char*));
    if (!map) {
        printf("Ошибка: не удалось выделить память под карту.\n");
        return NULL;
    }
    for (int i = 0; i < height; i++) {
        map[i] = (char*)malloc(width * sizeof(char));
        if (!map[i]) {
            printf("Ошибка: не удалось выделить память под строку %d.\n", i);
            for (int j = 0; j < i; j++) free(map[j]);
            free(map);
            return NULL;
        }
    }
    return map;
}

// Освобождение памяти (обратный порядок!)
void freeMap(char **map, int height) {
    if (!map) return;
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}

// Генерация этажа башни
int towerGenerate(TowerState *t) {
    // Размер
    t->height = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    t->width  = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    // Минимум 7x7, чтобы было где развернуться
    if (t->height < 7) t->height = 7;
    if (t->width  < 7) t->width  = 7;
    // Убедимся, что размеры чётные для симметрии (не обязательно, но удобно)
    if (t->height % 2 == 0) t->height++;
    if (t->width  % 2 == 0) t->width++;

    t->map = allocateMap(t->height, t->width);
    if (!t->map) return 0;

    // Всё — пол
    for (int y = 0; y < t->height; y++)
        for (int x = 0; x < t->width; x++)
            t->map[y][x] = TOWER_FLOOR;

    // Границы — стены
    for (int y = 0; y < t->height; y++) {
        t->map[y][0] = TOWER_WALL;
        t->map[y][t->width - 1] = TOWER_WALL;
    }
    for (int x = 0; x < t->width; x++) {
        t->map[0][x] = TOWER_WALL;
        t->map[t->height - 1][x] = TOWER_WALL;
    }

    // Ориентация разделительной стены
    t->split_vertical = rand() % 2;
    t->mirror_broken = 0;

    int obstacle_chance = TOWER_OBSTACLE_MIN +
        rand() % (TOWER_OBSTACLE_MAX - TOWER_OBSTACLE_MIN + 1);

    if (t->split_vertical) {
        // Вертикальная стена по центру
        t->split_pos = t->width / 2;

        // Препятствия в первой половине (x in [1, split_pos-1])
        for (int y = 1; y < t->height - 1; y++) {
            for (int x = 1; x < t->split_pos; x++) {
                if (rand() % 100 < obstacle_chance)
                    t->map[y][x] = TOWER_WALL;
            }
        }

        // Зеркальное отражение во вторую половину (относительно split_pos)
        int span = t->split_pos - 1;
        for (int y = 1; y < t->height - 1; y++) {
            for (int i = 0; i < span; i++) {
                int x_left  = t->split_pos - 1 - i;
                int x_right = t->split_pos + 1 + i;
                if (x_right >= t->width - 1) break;
                t->map[y][x_right] = t->map[y][x_left];
            }
        }

        // Разделительная стена
        for (int y = 0; y < t->height; y++)
            t->map[y][t->split_pos] = TOWER_MIRROR;

    } else {
        // Горизонтальная стена по центру
        t->split_pos = t->height / 2;

        // Препятствия в первой (верхней) половине
        for (int y = 1; y < t->split_pos; y++) {
            for (int x = 1; x < t->width - 1; x++) {
                if (rand() % 100 < obstacle_chance)
                    t->map[y][x] = TOWER_WALL;
            }
        }

        // Зеркальное отражение во вторую (нижнюю) половину
        int span = t->split_pos - 1;
        for (int x = 1; x < t->width - 1; x++) {
            for (int i = 0; i < span; i++) {
                int y_top    = t->split_pos - 1 - i;
                int y_bottom = t->split_pos + 1 + i;
                if (y_bottom >= t->height - 1) break;
                t->map[y_bottom][x] = t->map[y_top][x];
            }
        }

        // Разделительная стена
        for (int x = 0; x < t->width; x++)
            t->map[t->split_pos][x] = TOWER_MIRROR_H;
    }

    // --- Герой на "своей" половине + лестница вниз под ним ---
    int placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (t->split_vertical) {
            x = 1 + rand() % (t->split_pos - 1);
            y = 1 + rand() % (t->height - 2);
        } else {
            x = 1 + rand() % (t->width - 2);
            y = 1 + rand() % (t->split_pos - 1);
        }
        if (t->map[y][x] == TOWER_FLOOR) {
            t->map[y][x] = TOWER_STAIR_DOWN;
            t->hero_x = x;
            t->hero_y = y;
            placed = 1;
        }
    }
    if (!placed) {
        // Fallback — первая свободная клетка на своей половине
        if (t->split_vertical) {
            for (int y = 1; y < t->height - 1 && !placed; y++)
                for (int x = 1; x < t->split_pos && !placed; x++)
                    if (t->map[y][x] == TOWER_FLOOR) {
                        t->map[y][x] = TOWER_STAIR_DOWN;
                        t->hero_x = x; t->hero_y = y; placed = 1;
                    }
        } else {
            for (int y = 1; y < t->split_pos && !placed; y++)
                for (int x = 1; x < t->width - 1 && !placed; x++)
                    if (t->map[y][x] == TOWER_FLOOR) {
                        t->map[y][x] = TOWER_STAIR_DOWN;
                        t->hero_x = x; t->hero_y = y; placed = 1;
                    }
        }
        if (!placed) return 0;
    }

    // --- Лестница вверх на своей половине ---
    placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (t->split_vertical) {
            x = 1 + rand() % (t->split_pos - 1);
            y = 1 + rand() % (t->height - 2);
        } else {
            x = 1 + rand() % (t->width - 2);
            y = 1 + rand() % (t->split_pos - 1);
        }
        if (t->map[y][x] == TOWER_FLOOR &&
            !(x == t->hero_x && y == t->hero_y)) {
            t->map[y][x] = TOWER_STAIR_UP;
            placed = 1;
        }
    }

    // --- Переключатель на своей половине ---
    placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (t->split_vertical) {
            x = 1 + rand() % (t->split_pos - 1);
            y = 1 + rand() % (t->height - 2);
        } else {
            x = 1 + rand() % (t->width - 2);
            y = 1 + rand() % (t->split_pos - 1);
        }
        if (t->map[y][x] == TOWER_FLOOR) {
            t->map[y][x] = TOWER_SWITCH;
            placed = 1;
        }
    }

    return 1;
}

// Освобождение башни
void towerFree(TowerState *t) {
    if (t->map) {
        freeMap(t->map, t->height);
        t->map = NULL;
    }
}

// Отрисовка (герой поверх клетки)
void towerPrint(const TowerState *t) {
    printf("\n===== Башня, этаж %d =====\n", t->floor);
    printf("Размер: %dx%d | Разделитель: %s | Переключатель: %s\n",
           t->width, t->height,
           t->split_vertical ? "вертикальный" : "горизонтальный",
           t->mirror_broken ? "НАЖАТ" : "не нажат");
    printf("\n");
    for (int y = 0; y < t->height; y++) {
        for (int x = 0; x < t->width; x++) {
            if (x == t->hero_x && y == t->hero_y)
                putchar(TOWER_HERO);
            else
                putchar(t->map[y][x]);
        }
        putchar('\n');
    }
    printf("W/A/S/D — движение, Q — выход домой (только на 'D')\n");
}

// Что под героем (символ карты в клетке героя)
static char towerCellUnderHero(const TowerState *t) {
    return t->map[t->hero_y][t->hero_x];
}

// Проверки лестниц
int towerCheckOnStairDown(const TowerState *t) {
    return towerCellUnderHero(t) == TOWER_STAIR_DOWN;
}
int towerCheckOnStairUp(const TowerState *t) {
    return towerCellUnderHero(t) == TOWER_STAIR_UP;
}

// Перемещение
int towerTryMove(TowerState *t, char dir) {
    int nx = t->hero_x, ny = t->hero_y;
    switch (dir) {
        case 'W': ny--; break;
        case 'S': ny++; break;
        case 'A': nx--; break;
        case 'D': nx++; break;
        default:
            printf("Неизвестное направление.\n");
            return 0;
    }

    if (nx < 0 || nx >= t->width || ny < 0 || ny >= t->height) {
        printf("За границей.\n");
        return 0;
    }

    char cell = t->map[ny][nx];

    // Стены и разделитель — нельзя
    if (cell == TOWER_WALL || cell == TOWER_MIRROR ||
        cell == TOWER_MIRROR_H) {
        printf("Стена — туда нельзя.\n");
        return 0;
    }

    // Проверка "своей половины" (если разделитель ещё цел)
    if (!t->mirror_broken) {
        if (t->split_vertical && nx > t->split_pos) {
            printf("Зеркальная стена не пускает на чужую половину.\n");
            return 0;
        }
        if (!t->split_vertical && ny > t->split_pos) {
            printf("Зеркальная стена не пускает на чужую половину.\n");
            return 0;
        }
    }

    // Двигаем
    t->hero_x = nx;
    t->hero_y = ny;

    // Обработка клетки, на которую встали
    if (cell == TOWER_SWITCH) {
        printf("Щёлк! Переключатель нажат — зеркальная стена исчезла!\n");
        t->mirror_broken = 1;
        if (t->split_vertical) {
            for (int y = 0; y < t->height; y++)
                if (t->map[y][t->split_pos] == TOWER_MIRROR)
                    t->map[y][t->split_pos] = TOWER_FLOOR;
        } else {
            for (int x = 0; x < t->width; x++)
                if (t->map[t->split_pos][x] == TOWER_MIRROR_H)
                    t->map[t->split_pos][x] = TOWER_FLOOR;
        }
        t->map[ny][nx] = TOWER_FLOOR;
    }

    return 1;
}

// Главный цикл приключения в башне
void towerAdventure(void) {
    TowerState t;
    memset(&t, 0, sizeof(t));
    t.floor = 1;

    if (!towerGenerate(&t)) {
        printf("Не удалось сгенерировать этаж.\n");
        towerFree(&t);
        return;
    }
    towerPrint(&t);

    int running = 1;
    while (running) {
        printf("\nХод: ");
        char buf[16];
        if (!fgets(buf, sizeof(buf), stdin)) break;
        char dir = buf[0];
        if (dir >= 'a' && dir <= 'z') dir -= 32;

        if (dir == 'Q') {
            if (towerCheckOnStairDown(&t)) {
                printf("Вы спускаетесь на лестнице вниз... и оказываетесь дома!\n");
                running = 0;
            } else {
                printf("Домой можно вернуться только стоя на лестнице вниз ('D').\n");
            }
            continue;
        }

        if (dir != 'W' && dir != 'A' && dir != 'S' && dir != 'D') {
            printf("Неизвестная команда.\n");
            continue;
        }

        towerTryMove(&t, dir);

        // Переход на этаж выше?
        if (towerCheckOnStairUp(&t)) {
            printf("\nВы поднимаетесь по лестнице на следующий этаж!\n");
            // Освобождаем старую карту ПЕРЕД генерацией новой
            towerFree(&t);
            t.floor++;
            t.mirror_broken = 0;
            if (!towerGenerate(&t)) {
                printf("Не удалось сгенерировать новый этаж. Возврат домой.\n");
                running = 0;
                break;
            }
        }

        towerPrint(&t);
    }

    towerFree(&t);
    printf("Память башни освобождена.\n");
}