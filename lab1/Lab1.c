//вариант 8
#include <stdio.h>

#define INVENTORY_SIZE   10
#define HOURS_IN_DAY     24
#define START_DAY        1
#define START_HOUR       8
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
//если я правльно понял я задал стартовые значения 



int current_day  = START_DAY;
int current_hour = START_HOUR;
//инт целые числа 
int inventory[INVENTORY_SIZE] = {0};
//толпа коробочек с индексом от 0 до 9
//= {0} это все идексы равны 0

int  readInt(int *value);
void printItemName(int id);
void showTime(void);
void workHours(void);
void showInventory(void);
void putItem(void);
void dropItem(void);
void variantTask(void);
//толпа прототипов которые зарания говорят компу чтобы он не сломался



int main(void) {
//пустой инвентарь


	int choice;

    while (1) {
    //бесконечный цикл с выбором?
        printf("\n===== МЕНЮ =====\n");
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Быстрый доступ (задание по варианту)\n");
        printf("Выбор: ");
	//ну вроде меню выбора 
        if (!readInt(&choice)) {
            printf("Ошибка: нужно ввести число.\n");
            continue;
     //ввод с защитой от буковок?
        }

        switch (choice) {
            case 0:
      //смотрим на значение от 1 до 5
                printf("Выход из игры.\n");
                return 0;
       //завершает весь main а значит и весь код
            case 1: showTime();      break;
       //часы
            case 2: workHours();     break;
       //скип время
            case 3: showInventory(); break;
       //чек инвентарь 
            case 4: putItem();       break;
       //положить предмет в слот
            case 5: dropItem();      break;
       //выкинуть предмет
			case 6: variantTask();   break; 
            default:
                printf("Нет такого пункта меню.\n");
                break;
                //если нет такого пункта пишем что нет такого пункта
        }
    }
    return 0;
}
int readInt(int *value) {
    if (scanf("%d", value) != 1) {
        while (getchar() != '\n');
        //чистим буфер ввода
        return 0;
    }
    return 1;
    
}
void printItemName(int id) {
    switch (id) {
        case ITEM_EMPTY: printf("пусто");       break;
        case ITEM_WOOD:  printf("дерево");      break;
        case ITEM_STONE: printf("камень");      break;
        case ITEM_SEEDS: printf("семена");      break;
        case ITEM_IRON:  printf("железо");      break;
        case ITEM_WATER: printf("вода");        break;
        case ITEM_FOOD:  printf("еда");         break;
        case ITEM_ROPE:  printf("веревка");     break;
        case ITEM_TORCH: printf("факел");       break;
        case ITEM_GOLD:  printf("золото");      break;
        default:         printf("неизвестно");  break;
        //толпа предметов которые очень нужны
    }
}
void showTime(void) {
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
    //%d — подставить целое число 
    //%02d — подставить целое, но минимум 2 цифры, лишнее дополнить нулём
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
        //чтоб не было отрицательных
    }

    current_hour += hours;
    while (current_hour >= HOURS_IN_DAY) {
        current_hour -= HOURS_IN_DAY;
        current_day++;
        //херь чтобы если работать 30 часов добавился день
    }

    printf("Прошло %d ч. ", hours);
    showTime();
}
void showInventory(void) {
    printf("\n--- Рюкзак ---\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: [%d] (", i, inventory[i]);
        printItemName(inventory[i]);
        printf(")\n");
        //смотрим инвентарь через счётчик
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

    printf("ID предмета (0..9): ");
    if (!readInt(&id)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (id < 0 || id > 9) {
        printf("Ошибка: такого ID нет.\n");
        return;
    //можно только от 1 до 9 
    }

    inventory[slot] = id;
    printf("В слот %d положено: ", slot);
    printItemName(id);
    printf("\n");
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
    //прирваниваем слот к 0 делая его пустым
}
void variantTask(void) {
    int id;
    int found_index = -1;   
    int temp;

    
    printf("Какой предмет ищем? Введите ID (0..9): ");
    if (!readInt(&id)) {
        printf("Ошибка: нужно ввести число.\n");
        return;
    }
    if (id < 0 || id > 9) {
        printf("Ошибка: такого ID нет.\n");
        return;
    }
    if (id == ITEM_EMPTY) {
        printf("Нельзя искать 'пусто' — выберите конкретный предмет.\n");
        return;
    }

    
    printf("\n--- Инвентарь ДО ---\n");
    showInventory();

    
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == id) {
            found_index = i;
            break;         
        }
    }

    
    if (found_index == -1) {
        printf("\nПредмет с ID %d не найден в рюкзаке.\n", id);
        return;
    }

    
    if (found_index == 0) {
        printf("\nПредмет уже находится в слоте 0, менять нечего.\n");
        return;
    }

    
    temp                   = inventory[0];
    inventory[0]           = inventory[found_index];
    inventory[found_index] = temp;

    printf("\nПредмет найден в слоте %d и перемещён в слот 0.\n", found_index);


    printf("\n--- Инвентарь ПОСЛЕ ---\n");
    showInventory();
}
