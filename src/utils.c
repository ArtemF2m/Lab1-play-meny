#include "utils.h"

char g_item_names[MAX_ITEMS][MAX_NAME_LEN];

int read_int(int *value) {
    if (scanf("%d", value) != 1) {
        while (getchar() != '\n');
        return 0;
    }
    return 1;
}

void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
    }
}

void load_item_names(void) {
    const char *defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена", "Железо",
        "Вода", "Еда", "Веревка", "Факел", "Золото", "Кирка"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(g_item_names[i], defaults[i], MAX_NAME_LEN - 1);
        g_item_names[i][MAX_NAME_LEN - 1] = '\0';
    }
    FILE *f = fopen("items.txt", "r");
    if (!f) return;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        int id;
        char name[MAX_NAME_LEN];
        if (sscanf(line, "%d %31s", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) {
                strncpy(g_item_names[id], name, MAX_NAME_LEN - 1);
                g_item_names[id][MAX_NAME_LEN - 1] = '\0';
            }
        }
    }
    fclose(f);
}

void clear_screen(void) {
    // В Linux: system("clear"); В Windows: system("cls");
    // Оставим просто отступ для совместимости
    printf("\n\n");
}