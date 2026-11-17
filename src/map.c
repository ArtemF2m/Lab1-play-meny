#include "map.h"
#include <time.h>

char **allocate_map(int height, int width) {
    char **map = (char**)malloc(height * sizeof(char*));
    if (!map) return NULL;
    for (int i = 0; i < height; i++) {
        map[i] = (char*)malloc(width * sizeof(char));
        if (!map[i]) {
            for (int j = 0; j < i; j++) free(map[j]);
            free(map);
            return NULL;
        }
    }
    return map;
}

void free_map(char **map, int height) {
    if (!map) return;
    for (int i = 0; i < height; i++) free(map[i]);
    free(map);
}

Map* map_create(int floor) {
    Map *m = (Map*)malloc(sizeof(Map));
    if (!m) return NULL;

    m->height = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    m->width  = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    if (m->height < 7) m->height = 7;
    if (m->width  < 7) m->width  = 7;
    if (m->height % 2 == 0) m->height++;
    if (m->width  % 2 == 0) m->width++;

    m->map = allocate_map(m->height, m->width);
    if (!m->map) { free(m); return NULL; }

    for (int y = 0; y < m->height; y++)
        for (int x = 0; x < m->width; x++)
            m->map[y][x] = TOWER_FLOOR;

    for (int y = 0; y < m->height; y++) {
        m->map[y][0] = TOWER_WALL;
        m->map[y][m->width - 1] = TOWER_WALL;
    }
    for (int x = 0; x < m->width; x++) {
        m->map[0][x] = TOWER_WALL;
        m->map[m->height - 1][x] = TOWER_WALL;
    }

    m->split_vertical = rand() % 2;
    m->mirror_broken = 0;
    m->floor = floor;

    int obstacle_chance = TOWER_OBSTACLE_MIN +
        rand() % (TOWER_OBSTACLE_MAX - TOWER_OBSTACLE_MIN + 1);

    if (m->split_vertical) {
        m->split_pos = m->width / 2;
        for (int y = 1; y < m->height - 1; y++) {
            for (int x = 1; x < m->split_pos; x++) {
                int r = rand() % 100;
                if (r < obstacle_chance) m->map[y][x] = TOWER_WALL;
                else if (r < obstacle_chance + 10) m->map[y][x] = '#';
                else if (r < obstacle_chance + 15) m->map[y][x] = 'R';
            }
        }
        for (int y = 1; y < m->height - 1; y++) {
            for (int x = m->split_pos + 1; x < m->width - 1; x++) {
                int r = rand() % 100;
                if (r < obstacle_chance) m->map[y][x] = TOWER_WALL;
                else if (r < obstacle_chance + 10) m->map[y][x] = '#';
                else if (r < obstacle_chance + 15) m->map[y][x] = 'R';
            }
        }
        for (int y = 0; y < m->height; y++) m->map[y][m->split_pos] = TOWER_MIRROR;
    } else {
        m->split_pos = m->height / 2;
        for (int y = 1; y < m->split_pos; y++) {
            for (int x = 1; x < m->width - 1; x++) {
                int r = rand() % 100;
                if (r < obstacle_chance) m->map[y][x] = TOWER_WALL;
                else if (r < obstacle_chance + 10) m->map[y][x] = '#';
                else if (r < obstacle_chance + 15) m->map[y][x] = 'R';
            }
        }
        for (int y = m->split_pos + 1; y < m->height - 1; y++) {
            for (int x = 1; x < m->width - 1; x++) {
                int r = rand() % 100;
                if (r < obstacle_chance) m->map[y][x] = TOWER_WALL;
                else if (r < obstacle_chance + 10) m->map[y][x] = '#';
                else if (r < obstacle_chance + 15) m->map[y][x] = 'R';
            }
        }
        for (int x = 0; x < m->width; x++) m->map[m->split_pos][x] = TOWER_MIRROR_H;
    }

    int placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (m->split_vertical) { x = 1 + rand() % (m->split_pos - 1); y = 1 + rand() % (m->height - 2); }
        else { x = 1 + rand() % (m->width - 2); y = 1 + rand() % (m->split_pos - 1); }
        if (m->map[y][x] == TOWER_FLOOR) {
            m->map[y][x] = TOWER_STAIR_DOWN;
            m->hero_x = x; m->hero_y = y; placed = 1;
        }
    }
    if (!placed) { map_free(&m); return NULL; }

    placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (m->split_vertical) { x = 1 + rand() % (m->split_pos - 1); y = 1 + rand() % (m->height - 2); }
        else { x = 1 + rand() % (m->width - 2); y = 1 + rand() % (m->split_pos - 1); }
        if (m->map[y][x] == TOWER_FLOOR) { m->map[y][x] = TOWER_SWITCH; placed = 1; }
    }

    placed = 0;
    for (int attempts = 0; attempts < 2000 && !placed; attempts++) {
        int x, y;
        if (m->split_vertical) { x = m->split_pos + 1 + rand() % (m->width - m->split_pos - 2); y = 1 + rand() % (m->height - 2); }
        else { x = 1 + rand() % (m->width - 2); y = m->split_pos + 1 + rand() % (m->height - m->split_pos - 2); }
        if (m->map[y][x] == TOWER_FLOOR) { m->map[y][x] = TOWER_STAIR_UP; placed = 1; }
    }

    if (m->split_vertical) { m->mirror_hero_x = m->width - 1 - m->hero_x; m->mirror_hero_y = m->hero_y; }
    else { m->mirror_hero_x = m->hero_x; m->mirror_hero_y = m->height - 1 - m->hero_y; }

    m->entity_count = entity_generate(&m->entities, m->map, m->width, m->height, floor);
    m->drops = (Drop*)malloc(MAX_DROPS * sizeof(Drop));
    m->drop_count = 0;

    return m;
}

void map_free(Map **m) {
    if (m && *m) {
        free_map((*m)->map, (*m)->height);
        entity_free(&(*m)->entities);
        if ((*m)->drops) {
            printf("Память массива дропа по адресу %p освобождена.\n", (void*)(*m)->drops);
            free((*m)->drops);
        }
        printf("Память структуры карты по адресу %p освобождена.\n", (void*)*m);
        free(*m);
        *m = NULL;
    }
}

void map_print(const Map *m, const Player *p) {
    printf("\n===== Башня, этаж %d =====\n", m->floor);
    printf("HP: %d/%d | Стамина: %d/%d | Золото: %d\n",
           p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold);
    
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            char to_print = m->map[y][x];

            if (!m->mirror_broken) {
                if (m->split_vertical && x > m->split_pos) {
                    int mx = m->width - 1 - x;
                    if (mx == m->hero_x && y == m->hero_y) to_print = TOWER_MIRROR_HERO;
                    else to_print = m->map[y][mx];
                    putchar(to_print);
                    continue;
                }
                if (!m->split_vertical && y > m->split_pos) {
                    int my = m->height - 1 - y;
                    if (x == m->hero_x && my == m->hero_y) to_print = TOWER_MIRROR_HERO;
                    else to_print = m->map[my][x];
                    putchar(to_print);
                    continue;
                }
            }

            if (x == m->hero_x && y == m->hero_y) {
                to_print = TOWER_HERO;
            } else {
                int found = 0;
                for (int i = 0; i < m->entity_count; i++) {
                    if (m->entities[i].is_alive && m->entities[i].x == x && m->entities[i].y == y) {
                        to_print = m->entities[i].symbol;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    for (int i = 0; i < m->drop_count; i++) {
                        if (m->drops[i].is_alive && m->drops[i].x == x && m->drops[i].y == y) {
                            if (m->drops[i].item_id == ITEM_WOOD) to_print = DROP_LOG;
                            else if (m->drops[i].item_id == ITEM_STONE) to_print = DROP_STONE;
                            else if (m->drops[i].item_id == ITEM_GOLD) to_print = DROP_GOLD;
                            break;
                        }
                    }
                }
            }
            putchar(to_print);
        }
        putchar('\n');
    }
}

int map_move(Map *m, char dir) {
    int nx = m->hero_x, ny = m->hero_y;
    switch (dir) {
        case 'W': ny--; break;
        case 'S': ny++; break;
        case 'A': nx--; break;
        case 'D': nx++; break;
        default: return 0;
    }

    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) return 0;

    // Проверка на живую сущность (чтобы не наступить на камень/дерево)
    for (int i = 0; i < m->entity_count; i++) {
        if (m->entities[i].is_alive && m->entities[i].x == nx && m->entities[i].y == ny) {
            return 0;
        }
    }

    char cell = m->map[ny][nx];
    if (cell == TOWER_WALL || cell == TOWER_MIRROR || cell == TOWER_MIRROR_H) return 0;

    if (!m->mirror_broken) {
        if (m->split_vertical && nx > m->split_pos) return 0;
        if (!m->split_vertical && ny > m->split_pos) return 0;
    }

    m->hero_x = nx; m->hero_y = ny;
    
    if (m->split_vertical) { m->mirror_hero_x = m->width - 1 - m->hero_x; m->mirror_hero_y = m->hero_y; }
    else { m->mirror_hero_x = m->hero_x; m->mirror_hero_y = m->height - 1 - m->hero_y; }

    if (cell == TOWER_SWITCH) {
        m->mirror_broken = 1;
        if (m->split_vertical) {
            for (int y = 1; y < m->height - 1; y++)
                if (m->map[y][m->split_pos] == TOWER_MIRROR) m->map[y][m->split_pos] = TOWER_FLOOR;
        } else {
            for (int x = 1; x < m->width - 1; x++)
                if (m->map[m->split_pos][x] == TOWER_MIRROR_H) m->map[m->split_pos][x] = TOWER_FLOOR;
        }
        m->map[ny][nx] = TOWER_FLOOR;
    }
    return 1;
}