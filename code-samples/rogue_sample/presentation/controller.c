#include "controller.h"

static bool check_death(const player_t *player)
{
    return player->base_stats.health <= 0;
}

static bool check_end(const level_t *level)
{
    return level->level_num > LEVEL_NUM;
}

static bool check_level_end(const level_t *level, const player_t *player)
{
    return check_equal_coords(level->end_of_level.coordinates, player->base_stats.coords.coordinates);
}

static void print_weapon_menu(const player_t *player)
{
    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - 30) / 2;
    int shift_y = (row - 10) / 2;

    size_t count_weapon = player->backpack.weapon_num;
    move(shift_y - 1, shift_x);
    printw("Choose weapon:\n");
    if (count_weapon)
    {
        move(shift_y, shift_x);
        printw("0. Without weapon\n");
    }
    for (size_t i = 1; i <= count_weapon; i++)
    {
        move(shift_y + i, shift_x);
        printw("%llu. %s %+d strength\n", (unsigned long long)i, player->backpack.weapons[i - 1].name, player->backpack.weapons[i - 1].strength);
    }
    if (!count_weapon)
    {
        move(shift_y + 1, shift_x);
        printw("You haven't weapon!\n");
        move(shift_y + 2, shift_x);
        printw("Press any key to continue...\n");
    }
    else
    {
        move(shift_y + count_weapon + 1, shift_x);
        printw("Press 1-%llu key to choose weapon or any key to continue", (unsigned long long)count_weapon);
    }
}

static void print_food_menu(const player_t *player)
{
    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - 30) / 2;
    int shift_y = (row - 10) / 2;
    
    size_t count_food = player->backpack.food_num;
    move(shift_y, shift_x);
    printw("Choose food:\n");
    for (size_t i = 1; i <= count_food; i++)
    {
        move(shift_y + i, shift_x);
        printw("%llu. %s %+d health\n", (unsigned long long)i, player->backpack.foods[i - 1].name, player->backpack.foods[i - 1].to_regen);
    }
    if (!count_food)
    {
        move(shift_y + 1, shift_x);
        printw("You haven't food!\n");
        move(shift_y + 2, shift_x);
        printw("Press any key to continue...\n");
    }
    else
    {
        move(shift_y + count_food + 1, shift_x);
        printw("Press 1-%llu key to choose food or any key to continue", (unsigned long long)count_food);
    }
}

static void print_scroll_menu(const player_t *player)
{
    static const char *scroll_type[] = { "health", "agility", "strength" };

    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - 30) / 2;
    int shift_y = (row - 10) / 2;
    
    size_t count_scroll = player->backpack.scroll_num;
    move(shift_y, shift_x);
    printw("Choose scroll:\n");
    for (size_t i = 1; i <= count_scroll; i++)
    {
        move(shift_y + i, shift_x);
        const char *string = scroll_type[player->backpack.scrolls[i - 1].stat];
        printw("%llu. %s %+d %s\n", (unsigned long long)i, player->backpack.scrolls[i - 1].name, player->backpack.scrolls[i - 1].increase, string);
    }
    if (!count_scroll)
    {
        move(shift_y + 1, shift_x);
        printw("You haven't scroll!\n");
        move(shift_y + 2, shift_x);
        printw("Press any key to continue...\n");
    }
    else
    {
        move(shift_y + count_scroll + 1, shift_x);
        printw("Press 1-%llu key to choose scroll or any key to continue\n", (unsigned long long)count_scroll);
    }
}

static void print_elixir_menu(const player_t *player)
{
    static const char *scroll_type[] = { "health", "agility", "strength" };

    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - 30) / 2;
    int shift_y = (row - 10) / 2;
    
    size_t count_elixir = player->backpack.elixir_num;
    move(shift_y, shift_x);
    printw("Choose elixir:\n");
    for (size_t i = 1; i <= count_elixir; i++)
    {
        move(shift_y + i, shift_x);
        const char *string = scroll_type[player->backpack.elixirs[i - 1].stat];
        printw("%llu. %s %+d %s for %lld seconds\n", (unsigned long long)i, player->backpack.elixirs[i - 1].name, player->backpack.elixirs[i - 1].increase, string, (long long)player->backpack.elixirs[i - 1].duration);
    }
    if (!count_elixir)
    {
        move(shift_y + 1, shift_x);
        printw("You haven't elixir!\n");
        move(shift_y + 2, shift_x);
        printw("Press any key to continue...\n");
    }
    else
    {
        move(shift_y + count_elixir + 1, shift_x);
        printw("Press 1-%llu key to choose elixir or any key to continue\n", (unsigned long long)count_elixir);
    }
}

static void choose_consumable(player_t *player, consumable_types_e type, room_t *room)
{
    clear();
    int count_consumable = 0;
    switch (type)
    {
        case WEAPON:
            print_weapon_menu(player);
            count_consumable = player->backpack.weapon_num;
            break;
        case FOOD:
            print_food_menu(player);
            count_consumable = player->backpack.food_num;
            break;
        case SCROLL:
            print_scroll_menu(player);
            count_consumable = player->backpack.scroll_num;
            break;
        case ELIXIR:
            print_elixir_menu(player);
            count_consumable = player->backpack.elixir_num;
            break;
        default:
            return;
    }
    int key = getch() - '0';
    if (1 <= key && key <= count_consumable && (type != WEAPON || room))
        use_consumable(player, type, key - 1, room);
    if (key == 0 && type == WEAPON)
        use_consumable(player, type, key - 1, room);
    clear();
}

static void attack_monster_ui(const monster_t *monster, bool was_attack)
{
    static const char *monster_types[] = { "Zombie", "Vampire", "Ghost", "Ogre", "Snake" };

    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - MAP_WIDTH) / 2;
    int shift_y = (row - MAP_HEIGHT) / 2;
    move(shift_y - 2, shift_x);
    
    if (was_attack)
        printw("You attacked %s!!!", monster_types[monster->type]);
    else
        printw("You missed...");
}

static void check_consumable_ui(player_t *player, level_t *level)
{
    backpack_t old_backpack = player->backpack;
    
    for (size_t room = 0; room < ROOMS_NUM; room++)
        check_consumable(player, level->rooms + room);
    
    char *type = "unknown";
    if (old_backpack.food_num != player->backpack.food_num)
        type = "food";
    else if (old_backpack.weapon_num != player->backpack.weapon_num)
        type = "weapon";
    else if (old_backpack.scroll_num != player->backpack.scroll_num)
        type = "scroll";
    else if (old_backpack.elixir_num != player->backpack.elixir_num)
        type = "elixir";
        
    if (strcmp(type, "unknown") != 0)
    {
        int row, col;
        getmaxyx(stdscr, row, col);
        
        int shift_x = (col - MAP_WIDTH) / 2;
        int shift_y = (row - MAP_HEIGHT) / 2;
        move(shift_y - 2, shift_x);
        printw("You take the %s!!!", type);
    }
}

static void process_player_move_ui(player_t *player, level_t *level, battle_info_t *battles, directions_e direction, session_stat *stat)
{
    bool attacked = false;
    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
    {
        if (battles[i].is_fight)
        {
            double monster_health_before = battles[i].enemy->base_stats.health;
            if (check_player_attack(player, battles + i, direction))
            {
                stat->attacks++;
                attacked = true;
                attack_monster_ui(battles[i].enemy, battles[i].enemy->base_stats.health < monster_health_before);
                if (battles[i].enemy->base_stats.health <= 0)
                    stat->enemies++;
            }
        }
    }
    if (!attacked)
    {
        move_player(player, level, direction);
        check_consumable_ui(player, level);
    }
}

static void print_data_about_monster_attack(const monster_t *monster, bool was_attack)
{
    static const char *monster_types[] = { "Zombie", "Vampire", "Ghost", "Ogre", "Snake" };

    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - MAP_WIDTH) / 2;
    int shift_y = (row - MAP_HEIGHT) / 2;
    move(shift_y - 1, shift_x);
    
    if (was_attack)
        printw("%s attacked!!!", monster_types[monster->type]);
    else if (monster->type != OGRE)
        printw("%s missed!!!", monster_types[monster->type]);
}

static void process_monsters_move_ui(player_t *player, level_t *level, battle_info_t *battles, session_stat *stat)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
            if (check_unique(level->rooms[i].monsters + j, battles))
                move_monster(level->rooms[i].monsters + j, &player->base_stats.coords, level);
    
    remove_dead_monsters(level);
    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
        if (battles[i].is_fight)
        {
            double player_health_before = player->base_stats.health;
            attack(player, battles + i, MONSTER);
            print_data_about_monster_attack(battles[i].enemy, player->base_stats.health < player_health_before);
            if (player->base_stats.health < player_health_before)
                stat->missed++;
        }
}

static void process_user_move(player_t *player, level_t *level, battle_info_t *battles, directions_e direction, session_stat *stat)
{
    update_fight_status(&player->base_stats.coords, level, battles);
    process_player_move_ui(player, level, battles, direction, stat);
    process_monsters_move_ui(player, level, battles, stat);
    check_temp_effect_end(player);
}

static bool process_user_input(player_t *player, level_t *level, battle_info_t *battles, const char *filename)
{
    session_stat stat = get_current_stat(filename);
    
    stat.treasures = player->backpack.treasures.value;
    stat.level = level->level_num;

    int key = getch();
    
    int row, col;
    getmaxyx(stdscr, row, col);
    
    int shift_x = (col - MAP_WIDTH) / 2;
    int shift_y = (row - MAP_HEIGHT) / 2;
    move(shift_y - 1, shift_x); clrtoeol();
    move(shift_y - 2, shift_x); clrtoeol();

    bool quit = false;
    size_t current_count = 0;
    switch (key)
    {
        case 'W':
        case 'w':
            process_user_move(player, level, battles, FORWARD, &stat);
            stat.moves++;
            break;
        case 'A':
        case 'a':
            process_user_move(player, level, battles, LEFT, &stat);
            stat.moves++;
            break;
        case 'S':
        case 's':
            process_user_move(player, level, battles, BACK, &stat);
            stat.moves++;
            break;
        case 'D':
        case 'd':
            process_user_move(player, level, battles, RIGHT, &stat);
            stat.moves++;
            break;
        case 'H':
        case 'h':
            choose_consumable(player, WEAPON, find_current_room(&player->base_stats.coords, level));
            break;
        case 'J':
        case 'j':
            current_count = player->backpack.food_num;
            choose_consumable(player, FOOD, NULL);
            if (current_count != player->backpack.food_num)
                stat.food++;
            break;
        case 'K':
        case 'k':
            current_count = player->backpack.elixir_num;
            choose_consumable(player, ELIXIR, NULL);
            if (current_count != player->backpack.elixir_num)
                stat.elixirs++;
            break;
        case 'E':
        case 'e':
            current_count = player->backpack.scroll_num;
            choose_consumable(player, SCROLL, NULL);
            if (current_count != player->backpack.scroll_num)
                stat.scrolls++;
            break;
        case ESCAPE:
            quit = true;
            break;
    }

    save_session_stat(&stat, filename);
    return quit;
}

void game_cycle(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *save, const char *score, const char *stat)
{
    bool running = true;
    while (running)
    {
        display_map(map, level, player, battles);
        if (process_user_input(player, level, battles, stat))
            running = false;

        if (check_level_end(level, player))
        {
            clear_map_data(map);
            generate_next_level(level, player);
        }
        if (check_death(player))
        {
            running = false;
            update_statistics(score, stat);
            get_standart_save(save, stat);
            dead_screen();
        }
        if (check_end(level))
        {
            running = false;
            update_statistics(score, stat);
            get_standart_save(save, stat);
            endgame_screen();
        }

        if (running)
            save_data(player, level, map, save);
    }
}
