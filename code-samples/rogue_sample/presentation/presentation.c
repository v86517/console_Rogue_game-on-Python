#include "presentation.h"

#define CHANCE_UNVISIBLE_GHOST 80

void init_presentation(void)
{
    initscr();
    noecho();
    curs_set(0);
    start_color();
    keypad(stdscr, true);
    
    init_pair(WHITE_FONT,  COLOR_WHITE,  COLOR_BLACK);
    init_pair(RED_FONT,    COLOR_RED,    COLOR_BLACK);
    init_pair(GREEN_FONT,  COLOR_GREEN,  COLOR_BLACK);
    init_pair(BLUE_FONT,   COLOR_BLUE,   COLOR_BLACK);
    init_pair(YELLOW_FONT, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CYAN_FONT,   COLOR_CYAN,   COLOR_BLACK);
}

void clear_map_data(map_t *map)
{
    for (size_t i = 0; i < MAP_HEIGHT; i++)
        for (size_t j = 0; j < MAP_WIDTH; j++)
            map->map[i][j] = ' ';
    for (size_t i = 0; i < ROOMS_NUM; i++)
        map->visible_rooms[i] = false;
    for (size_t i = 0; i < MAX_PASSAGES_NUM; i++)
        map->visible_passages[i] = false;
}

static int get_room_by_coord(const object_t *coords, const room_t *rooms)
{
    int x = coords->coordinates[X];
    int y = coords->coordinates[Y];
    
    for (int room = 0; room < ROOMS_NUM; room++)
    {
        int x_room = rooms[room].coords.coordinates[X];
        int y_room = rooms[room].coords.coordinates[Y];
        
        int xsize = rooms[room].coords.size[X];
        int ysize = rooms[room].coords.size[Y];
        
        int checkx = (x >= x_room) && (x < x_room + xsize);
        int checky = (y >= y_room) && (y < y_room + ysize);
        if (checky && checkx)
            return room;
    }

    return -1;
}

static void rooms_to_map(map_t *map, const room_t *rooms, const player_t *player)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        if (!map->visible_rooms[i] && get_room_by_coord(&player->base_stats.coords, rooms) != (signed)i)
            continue;
            
        int x1 = rooms[i].coords.coordinates[X];
        int y1 = rooms[i].coords.coordinates[Y];
        
        int xsize = rooms[i].coords.size[X];
        int ysize = rooms[i].coords.size[Y];
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                int checkx = (x == x1 || x == x1 + xsize - 1) && (y1 <= y && y < y1 + ysize);
                int checky = (y == y1 || y == y1 + ysize - 1) && (x1 <= x && x < x1 + xsize);
                if (checky)
                    map->map[y][x] = '-';
                else if (checkx)
                    map->map[y][x] = '|';
            }
        }
        
        map->visible_rooms[i] = true;
    }
}

static void passages_to_map(map_t *map, const passages_t *passages, const room_t *rooms, const player_t *player)
{
    for (size_t i = 0; i < passages->passages_num; i++) {
        bool visible = true;
        if (!map->visible_passages[i] && character_outside_border(&player->base_stats.coords, passages->passages + i))
            visible = false;
    
        int x1 = passages->passages[i].coordinates[X];
        int y1 = passages->passages[i].coordinates[Y];
        
        int xsize = passages->passages[i].size[X];
        int ysize = passages->passages[i].size[Y];
        
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                int checkx = (x1 < x && x < x1 + xsize - 1) && (y1 < y && y < y1 + ysize - 1);
                object_t coords = { .coordinates[X] = x, .coordinates[Y] = y };
                int room = get_room_by_coord(&coords, rooms);
                if (checkx && visible)
                {
                    if (room != -1)
                        map->map[y][x] = '+';
                    else
                        map->map[y][x] = '#';
                }
                else if (checkx && room != -1 && map->visible_rooms[room])
                    map->map[y][x] = '+';
            }
        }
        
        map->visible_passages[i] = visible;
    }
}

static bool unvisible_ghost(const monster_t *monster, const battle_info_t *battles)
{
    bool unvisible = false;
    if (rand() % 100 < CHANCE_UNVISIBLE_GHOST)
        unvisible = true;
    if (!check_unique(monster, battles))
        unvisible = false;
    return unvisible;
}

static bool on_the_same_room_or_passage(const level_t *level, const object_t *character_coords, const object_t *monster_coords)
{
    bool same = false;
    for (size_t i = 0; i < ROOMS_NUM && !same; i++)
        if (!character_outside_border(character_coords, &level->rooms[i].coords) &&
                !character_outside_border(monster_coords, &level->rooms[i].coords))
            same = true;
    for (size_t i = 0; i < level->passages.passages_num && !same; i++)
        if (!character_outside_border(character_coords, &level->passages.passages[i]) &&
                !character_outside_border(monster_coords, &level->passages.passages[i]))
            same = true;
    return same;
}

static void monsters_to_map(map_t *map, const level_t *level, const player_t *player, const battle_info_t *battles)
{
    static const char *monster_letters = "zvgOs";
    static const font_e monster_colors[] = { GREEN_FONT, RED_FONT, WHITE_FONT, YELLOW_FONT, WHITE_FONT };

    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
        {
            if (on_the_same_room_or_passage(level, &player->base_stats.coords, &level->rooms[i].monsters[j].base_stats.coords) ||
                    !check_unique(level->rooms[i].monsters + j, battles))
            {
                int x = level->rooms[i].monsters[j].base_stats.coords.coordinates[X];
                int y = level->rooms[i].monsters[j].base_stats.coords.coordinates[Y];

                monster_type_e type = level->rooms[i].monsters[j].type;
                map->map[y][x] = monster_letters[type] | COLOR_PAIR(monster_colors[type]);
                if (type == GHOST && unvisible_ghost(level->rooms[i].monsters + j, battles))
                    map->map[y][x] = ' ' | COLOR_PAIR(monster_colors[type]);
            }
        }
    }
}

static int consumable_color(stat_type_e stat)
{
    int color = WHITE_FONT;
    switch (stat)
    {
        case HEALTH:
            color = RED_FONT;
            break;
        case STRENGTH:
            color = BLUE_FONT;
            break;
        case AGILITY:
            color = GREEN_FONT;
            break;
        default:
            break;
    }
    return color;
}

static void consumables_to_map(map_t *map, const room_t *rooms)
{
    for (size_t i = 0; i < ROOMS_NUM; i++) {
        for (size_t j = 0; j < rooms[i].consumables.food_num && map->visible_rooms[i]; j++) {
            int x = rooms[i].consumables.room_food[j].geometry.coordinates[X];
            int y = rooms[i].consumables.room_food[j].geometry.coordinates[Y];
            map->map[y][x] = 'f' | COLOR_PAIR(RED_FONT);
        }
        
        for (size_t j = 0; j < rooms[i].consumables.elixir_num && map->visible_rooms[i]; j++) {
            int x = rooms[i].consumables.elixirs[j].geometry.coordinates[X];
            int y = rooms[i].consumables.elixirs[j].geometry.coordinates[Y];
            
            int color = consumable_color(rooms[i].consumables.elixirs[j].elixir.stat);
            map->map[y][x] = 'e' | COLOR_PAIR(color);
        }
        
        for (size_t j = 0; j < rooms[i].consumables.scroll_num && map->visible_rooms[i]; j++) {
            int x = rooms[i].consumables.scrolls[j].geometry.coordinates[X];
            int y = rooms[i].consumables.scrolls[j].geometry.coordinates[Y];
            
            int color = consumable_color(rooms[i].consumables.scrolls[j].scroll.stat);
            map->map[y][x] = 'S' | COLOR_PAIR(color);
        }
        
        for (size_t j = 0; j < rooms[i].consumables.weapon_num && map->visible_rooms[i]; j++) {
            int x = rooms[i].consumables.weapons[j].geometry.coordinates[X];
            int y = rooms[i].consumables.weapons[j].geometry.coordinates[Y];
            map->map[y][x] = 'w' | COLOR_PAIR(BLUE_FONT);
        }
    }
}

static void exit_to_map(map_t *map, const level_t *level)
{
    int x = level->end_of_level.coordinates[X];
    int y = level->end_of_level.coordinates[Y];
    int room_end_of_level = get_room_by_coord(&level->end_of_level, level->rooms);
    if (map->visible_rooms[room_end_of_level])
        map->map[y][x] = 'E' | COLOR_PAIR(CYAN_FONT);
}

static void player_to_map(map_t *map, const player_t *player)
{
    int x = player->base_stats.coords.coordinates[X];
    int y = player->base_stats.coords.coordinates[Y];
    map->map[y][x] = '@' | COLOR_PAIR(WHITE_FONT);
}

static void fill_room_by_fog(map_t *map, const room_t *room)
{
    int x_room = room->coords.coordinates[X];
    int y_room = room->coords.coordinates[Y];
    
    int xsize = room->coords.size[X];
    int ysize = room->coords.size[Y];
    for (size_t x = (unsigned)x_room + 1; x < (unsigned)(x_room + xsize - 1); x++)
        for (size_t y = y_room + 1; y < (unsigned)(y_room + ysize - 1); y++)
            map->map[y][x] = '.';
}

static bool is_vertical_direction_fog(const object_t *coords, const object_t *room_coords)
{
    object_t new_coords = *coords;
    new_coords.coordinates[X]++;
    if (!character_outside_border(&new_coords, room_coords))
        return false;
    new_coords.coordinates[X] -= 2;
    if (!character_outside_border(&new_coords, room_coords))
        return false;
    return true;
}

static void fill_room_by_part_fog(map_t *map, const player_t *player, const room_t *room)
{
    int player_x = player->base_stats.coords.coordinates[X];
    int player_y = player->base_stats.coords.coordinates[Y];
    bool is_vertical = is_vertical_direction_fog(&player->base_stats.coords, &room->coords);
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            object_t coords = { .coordinates[X] = x, .coordinates[Y] = y, .size[X] = 1, .size[Y] = 1 };
            if (character_outside_border(&coords, &room->coords))
                continue;
            
            int new_x = x - player_x, new_y = y - player_y;
            if (is_vertical && abs(new_x) >= abs(new_y))
                map->map[y][x] = '.';
            if (!is_vertical && abs(new_x) <= abs(new_y))
                map->map[y][x] = '.';
        }
    }
}

static void fog_of_war_to_map(map_t *map, const level_t *level, const player_t *player)
{
    int room_player = get_room_by_coord(&player->base_stats.coords, level->rooms);
    for (size_t room = 0; room < ROOMS_NUM; room++)
    {
        if ((signed)room != room_player && map->visible_rooms[room])
            fill_room_by_fog(map, level->rooms + room);
        
        if ((signed)room == room_player && get_room_by_coord(&player->base_stats.coords, level->rooms) != -1 \
                && character_outside_border(&player->base_stats.coords, &level->rooms[room].coords))
            fill_room_by_part_fog(map, player, level->rooms + room);
    }
}

void create_new_map(map_t *map, const level_t *level, const player_t *player, const battle_info_t *battles)
{
    for (size_t i = 0; i < MAP_HEIGHT; i++)
        for (size_t j = 0; j < MAP_WIDTH; j++)
            map->map[i][j] = ' ';
    rooms_to_map(map, level->rooms, player);
    passages_to_map(map, &level->passages, level->rooms, player);
    monsters_to_map(map, level, player, battles);
    consumables_to_map(map, level->rooms);
    exit_to_map(map, level);
    player_to_map(map, player);
    fog_of_war_to_map(map, level, player);
}

void display_map(map_t *map, const level_t *level, const player_t *player, const battle_info_t *battles)
{
    create_new_map(map, level, player, battles);

    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - MAP_WIDTH) / 2;
    int shift_y = (row - MAP_HEIGHT) / 2;

    for (size_t i = 0; i < MAP_HEIGHT; i++)
    {
        move(shift_y + i, shift_x);
        for (size_t j = 0; j < MAP_WIDTH; j++)
            addch(map->map[i][j]);
        addch((chtype)'\n');
    }
    
    move(shift_y + MAP_HEIGHT, shift_x);
    printw("Level: %-8d ", level->level_num);
    printw("Gold: %-8d ", player->backpack.treasures.value);
    printw("Health: %.2lf/%-8d", player->base_stats.health, player->regen_limit);
    printw("Agility: %-6d ", player->base_stats.agility);
    printw("Strength: %d(+%d) ", player->base_stats.strength, player->weapon.strength);
    
    move(row, col);
    refresh();
}

static void print_line_ch(size_t width)
{
    for (size_t i = 0; i < width; i++)
        addch((chtype)'-');
}

void display_scoreboard(const char *path_scoreboard)
{
    int row, col;
    getmaxyx(stdscr, row, col);
    clear();

    size_t size_array = 0;
    session_stat *array_stats = get_session_stat_array(path_scoreboard, &size_array);
    qsort(array_stats, size_array, sizeof(session_stat), compare_session_stats);
    size_array = min(size_array, MAX_SCOREBOARD_SIZE);

    int field_size = 10;
    size_t width = field_size * 10;
    size_t height = size_array * 2 + 3;
    
    int shift_x = (col - width) / 2;
    int shift_y = (row - height) / 2;

    move(shift_y - 2, shift_x);
    print_line_ch(width);
    move(shift_y - 1, shift_x);
    printw("|%-*s", field_size, "treasures");
    printw("|%-*s", field_size, "level");
    printw("|%-*s", field_size, "enemies");
    printw("|%-*s", field_size, "food");
    printw("|%-*s", field_size, "elixirs");
    printw("|%-*s", field_size, "scrolls");
    printw("|%-*s", field_size, "attacks");
    printw("|%-*s", field_size, "missed");
    printw("|%-*s", field_size, "moves");
    printw("|\n");

    for (size_t i = 0; i < size_array; i++)
    {
        move(shift_y + 2 * i, shift_x);
        print_line_ch(width);
        move(shift_y + 2 * i + 1, shift_x);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].treasures);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].level);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].enemies);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].food);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].elixirs);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].scrolls);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].attacks);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].missed);
        printw("|%*llu", field_size, (unsigned long long) array_stats[i].moves);
        printw("|\n");
    }
    move(shift_y + 2 * size_array, shift_x);
    print_line_ch(width);

    move(shift_y + 2 * (size_array + 1), (col - 20) / 2);
    printw("Press ESCAPE to exit.");
    
    refresh();
    free(array_stats);
}
