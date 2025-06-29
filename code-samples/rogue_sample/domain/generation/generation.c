#include "generation.h"

void clear_data(level_t *level)
{
    for (size_t room = 0; room < ROOMS_NUM; room++)
    {
        level->rooms[room].monster_num = 0;
        level->rooms[room].consumables.food_num = 0;
        level->rooms[room].consumables.weapon_num = 0;
        level->rooms[room].consumables.elixir_num = 0;
        level->rooms[room].consumables.scroll_num = 0;
    }
}

void generate_next_level(level_t *level, player_t *player)
{
    clear_data(level);
    level->level_num++;
    generate_rooms(level->rooms);
    generate_passages(&level->passages, level->rooms);
    int player_room = generate_player(level->rooms, player);
    generate_monsters(level, player_room);
    generate_consumables(level, player_room, player, level->level_num);
    generate_exit(level, player_room);
}

void generate_rooms(room_t *rooms)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        int width_room = get_random_in_range(MIN_ROOM_WIDTH, MAX_ROOM_WIDTH);
        int height_room = get_random_in_range(MIN_ROOM_HEIGHT, MAX_ROOM_HEIGHT);
        
        // В зависимости от ширины комнаты определяются координаты,
        // в пределах которых может находится X координата
        int left_range_coord = (i % ROOMS_IN_WIDTH) * REGION_WIDTH + 1;
        int right_range_coord = (i % ROOMS_IN_WIDTH + 1) * REGION_WIDTH - width_room - 1;
        int x_coord = get_random_in_range(left_range_coord, right_range_coord);
        
        // Аналогично для Y координаты в зависимости от высоты комнаты
        int up_range_coord = (i / ROOMS_IN_WIDTH) * REGION_HEIGHT + 1;
        int bottom_range_coord = (i / ROOMS_IN_WIDTH + 1) * REGION_HEIGHT - height_room - 1;
        int y_coord = get_random_in_range(up_range_coord, bottom_range_coord);
        
        rooms[i].coords.size[X] = width_room;
        rooms[i].coords.size[Y] = height_room;
        
        rooms[i].coords.coordinates[X] = x_coord;
        rooms[i].coords.coordinates[Y] = y_coord;
    }
}

void generate_edges_for_rooms(edge_t *edges, size_t *count_edges)
{
    *count_edges = 0;
    
    // Генерация горизонтальных ребер между комнатами
    for (size_t i = 0; i < ROOMS_IN_HEIGHT; i++)
    {
        for (size_t j = 0; j + 1 < ROOMS_IN_WIDTH; j++)
        {
            int current_room = i * ROOMS_IN_HEIGHT + j;
            edges[*count_edges].u = current_room;
            edges[*count_edges].v = current_room + 1;
            (*count_edges)++;
        }
    }
    
    // Генерация вертикальных ребер между комнатами
    for (size_t i = 0; i + 1 < ROOMS_IN_HEIGHT; i++)
    {
        for (size_t j = 0; j < ROOMS_IN_WIDTH; j++)
        {
            int current_room = i * ROOMS_IN_HEIGHT + j;
            edges[*count_edges].u = current_room;
            edges[*count_edges].v = current_room + ROOMS_IN_WIDTH;
            (*count_edges)++;
        }
    }
}

void create_passage(int coord_x, int coord_y, int width, int height, passages_t *passages)
{
    passages->passages[passages->passages_num].coordinates[X] = coord_x - 1;
    passages->passages[passages->passages_num].coordinates[Y] = coord_y - 1;
    passages->passages[passages->passages_num].size[X] = width + 2;
    passages->passages[passages->passages_num].size[Y] = height + 2;
    passages->passages_num++;
}

void generate_horizontal_passage(int first_room, int second_room, const room_t *room, passages_t *passages)
{
    object_t first_coords = room[first_room].coords;
    object_t second_coords = room[second_room].coords;
    
    // Для первой комнаты фиксируется правая стена, поэтому X координата определяется однозначно
    // Для Y координаты определяются возможные значения и среди них выбирается случайное 
    int first_x            = first_coords.coordinates[X] + first_coords.size[X] - 1;
    int up_range_coord     = first_coords.coordinates[Y] + 1;
    int bottom_range_coord = first_coords.coordinates[Y] + first_coords.size[Y] - 2;
    int first_y            = get_random_in_range(up_range_coord, bottom_range_coord);
    
    // Аналогично для второй комнаты с левой стеной
    int second_x       = second_coords.coordinates[X];
    up_range_coord     = second_coords.coordinates[Y] + 1;
    bottom_range_coord = second_coords.coordinates[Y] + second_coords.size[Y] - 2;
    int second_y       = get_random_in_range(up_range_coord, bottom_range_coord);
    
    // Если Y координаты равны, то строится прямой коридор, иначе с изгибом,
    // место которого выбирается случайно
    if (first_y == second_y)
    {
        create_passage(first_x, first_y, abs(second_x - first_x) + 1, 1, passages);
    }
    else
    {
        int vertical = get_random_in_range(min(first_x, second_x) + 1, max(first_x, second_x) - 1);
        create_passage(first_x,  first_y,                abs(vertical - first_x) + 1, 1,  passages);
        create_passage(vertical, min(first_y, second_y), 1, abs(second_y - first_y) + 1,  passages);
        create_passage(vertical, second_y,               abs(second_x - vertical) + 1, 1, passages);
    }
}

void generate_vertical_passage(int first_room, int second_room, const room_t *room, passages_t *passages)
{
    object_t first_coords = room[first_room].coords;
    object_t second_coords = room[second_room].coords;
    
    // Для первой комнаты фиксируется нижняя стена, поэтому Y координата определяется однозначно
    // Для X координаты определяются возможные значения и среди них выбирается случайное
    int first_y            = first_coords.coordinates[Y] + first_coords.size[Y] - 1;
    int up_range_coord     = first_coords.coordinates[X] + 1;
    int bottom_range_coord = first_coords.coordinates[X] + first_coords.size[X] - 2;
    int first_x            = get_random_in_range(up_range_coord, bottom_range_coord);
    
    // Аналогично для второй комнаты с верхней стеной
    int second_y       = second_coords.coordinates[Y];
    up_range_coord     = second_coords.coordinates[X] + 1;
    bottom_range_coord = second_coords.coordinates[X] + second_coords.size[X] - 2;
    int second_x       = get_random_in_range(up_range_coord, bottom_range_coord);
    
    // Если X координаты равны, то строится прямой коридор, иначе с изгибом,
    // место которого выбирается случайно
    if (first_x == second_x)
    {
        create_passage(first_x, first_y, 1, abs(second_y - first_y) + 1, passages);
    }
    else
    {
        int horizont = get_random_in_range(min(first_y, second_y) + 1, max(first_y, second_y) - 1);
        create_passage(first_x,                first_y,  1, abs(horizont - first_y) + 1,  passages);
        create_passage(min(first_x, second_x), horizont, abs(second_x - first_x) + 1, 1,  passages);
        create_passage(second_x,               horizont, 1, abs(second_y - horizont) + 1, passages);
    }
}

void generate_passages(passages_t *passages, const room_t *rooms)
{
    // Создание массива ребер и получение его случайной перестановки
    passages->passages_num = 0;
    size_t count_passages;
    edge_t edges[MAX_PASSAGES_NUM];
    generate_edges_for_rooms(edges, &count_passages);
    shuffle_array((void *)edges, count_passages, sizeof(edge_t));
    
    // Коридоры между комнатами будут создаваться при помощи системы непересекающихся множеств
    // Будет сделан проход по всем возможным ребрам, и если очередная пара комнат не связана,
    // то между ними будет создан коридор
    int parent[ROOMS_NUM], rank[ROOMS_NUM];
    make_sets(parent, rank, ROOMS_NUM);
    for (size_t i = 0; i < count_passages; i++)
    {
        if (find_set(edges[i].u, parent) != find_set(edges[i].v, parent))
        {
            union_sets(edges[i].u, edges[i].v, parent, rank);
            if (abs(edges[i].u - edges[i].v) == 1)
                generate_horizontal_passage(edges[i].u, edges[i].v, rooms, passages);
            else
                generate_vertical_passage(edges[i].u, edges[i].v, rooms, passages);
        }
    }
}

void generate_coords_of_entity(const room_t *room, object_t *coords)
{
    int upper_left_x = room->coords.coordinates[X] + 1;
    int upper_left_y = room->coords.coordinates[Y] + 1;
    
    // Уменьшение идет на 3, чтобы координата гарантированно была внутри комнаты
    int bottom_right_x = upper_left_x + room->coords.size[X] - 3;
    int bottom_right_y = upper_left_y + room->coords.size[Y] - 3;
    
    coords->coordinates[X] = get_random_in_range(upper_left_x, bottom_right_x);
    coords->coordinates[Y] = get_random_in_range(upper_left_y, bottom_right_y);
    
    coords->size[X] = 1;
    coords->size[Y] = 1;
}

int generate_player(const room_t *rooms, player_t *player)
{
    int player_room = get_random_in_range(0, ROOMS_NUM - 1);
    generate_coords_of_entity(rooms + player_room, &player->base_stats.coords);
    return player_room;
}

void generate_exit(level_t *level, int player_room)
{
    int exit_room;
    do
    {
        exit_room = get_random_in_range(0, ROOMS_NUM - 1);
        while (exit_room == player_room)
            exit_room = get_random_in_range(0, ROOMS_NUM - 1);
        
        // Увеличение идет на 2, чтобы выход не был рядом со стеной комнаты
        // во избежание случаев, когда он будет находиться прямо перед коридором
        int upper_left_x = level->rooms[exit_room].coords.coordinates[X] + 2;
        int upper_left_y = level->rooms[exit_room].coords.coordinates[Y] + 2;
        
        // Уменьшение идет на 5, чтобы выход не был рядом со стеной комнаты
        int bottom_right_x = upper_left_x + level->rooms[exit_room].coords.size[X] - 5;
        int bottom_right_y = upper_left_y + level->rooms[exit_room].coords.size[Y] - 5;
        
        level->end_of_level.coordinates[X] = get_random_in_range(upper_left_x, bottom_right_x);
        level->end_of_level.coordinates[Y] = get_random_in_range(upper_left_y, bottom_right_y);
        
        level->end_of_level.size[X] = 1;
        level->end_of_level.size[Y] = 1;
    }
    while (!check_unoccupied_room(&level->end_of_level, level->rooms + exit_room));
}

void generate_monster_data(monster_t *monster, int level_num)
{
    monster->type = get_random_in_range(0, MONSTER_TYPE_NUM - 1);
    switch ((int) monster->type)
    {
        case ZOMBIE:
            monster->hostility = AVERAGE;
            monster->base_stats.agility = 25;
            monster->base_stats.strength = 125;
            monster->base_stats.health = 50;
            break;
        case VAMPIRE:
            monster->hostility = HIGH;
            monster->base_stats.agility = 75;
            monster->base_stats.strength = 125;
            monster->base_stats.health = 50;
            break;
        case GHOST:
            monster->hostility = LOW;
            monster->base_stats.agility = 75;
            monster->base_stats.strength = 25;
            monster->base_stats.health = 75;
            break;
        case OGRE:
            monster->hostility = AVERAGE;
            monster->base_stats.agility = 25;
            monster->base_stats.strength = 100;
            monster->base_stats.health = 150;
            break;
        case SNAKE:
            monster->hostility = HIGH;
            monster->base_stats.agility = 100;
            monster->base_stats.strength = 30;
            monster->base_stats.health = 100;
            break;
    }
    int percents_update = PERCENTS_UPDATE_DIFFICULTY_MONSTERS * level_num;
    monster->base_stats.agility += monster->base_stats.agility * percents_update / 100;
    monster->base_stats.strength += monster->base_stats.strength * percents_update / 100;
    monster->base_stats.health += monster->base_stats.health * percents_update / 100;

    monster->is_chasing = false;
    monster->dir = STOP;
}

void generate_monsters(level_t *level, int player_room)
{
    // Каждые LEVEL_UPDATE_DIFFICULTY количество монстров будет увеличиваться
    int max_monsters = MAX_MONSTERS_PER_ROOM + level->level_num / LEVEL_UPDATE_DIFFICULTY;
    for (int room = 0; room < ROOMS_NUM; room++)
    {
        if (room == player_room)
            continue;
            
        int count_monsters = get_random_in_range(0, max_monsters);
        for (int i = 0; i < count_monsters; i++)
        {
            object_t *coords = &level->rooms[room].monsters[i].base_stats.coords;
            do
            {
                generate_coords_of_entity(level->rooms + room, coords);
            }
            while (!check_unoccupied_room(coords, level->rooms + room));
            generate_monster_data(level->rooms[room].monsters + i, level->level_num);
            level->rooms[room].monster_num++;
        }
    }
}

void generate_food_data(food_t *food, const player_t *player)
{
    static const char *names[CONSUMABLES_TYPE_MAX_NUM] = {
        "Ration of the Ironclad",
        "Crimson Berry Cluster",
        "Loaf of the Forgotten Baker",
        "Smoked Wyrm Jerky",
        "Golden Apple of Vitality",
        "Hardtack of the Endless March",
        "Spiced Venison Strips",
        "Honeyed Nectar Bread",
        "Dried Mushrooms of the Deep",
    };

    int max_regen = player->base_stats.health * MAX_PERCENT_FOOD_REGEN_FROM_HEALTH / 100;
    food->to_regen = get_random_in_range(1, max_regen);

    int name_pos = get_random_in_range(0, CONSUMABLES_TYPE_MAX_NUM - 1);
    strcpy(food->name, names[name_pos]);
}

void generate_food(room_t *room, const player_t *player)
{
    int count_food = room->consumables.food_num;
    object_t *coords = &room->consumables.room_food[count_food].geometry;
    do
    {
        generate_coords_of_entity(room, coords);
    }
    while (!check_unoccupied_room(coords, room));
    generate_food_data(&room->consumables.room_food[count_food].food, player);
    room->consumables.food_num++;
}

void generate_elixir_data(elixir_t *elixir, const player_t *player)
{
    static const char *names[CONSUMABLES_TYPE_MAX_NUM] = {
        "Elixir of the Jade Serpent",
        "Potion of the Phantom's Breath",
        "Vial of Crimson Vitality",
        "Draught of the Frozen Star",
        "Elixir of the Shattered Mind",
        "Potion of the Wandering Soul",
        "Vial of Ember Essence",
        "Elixir of the Obsidian Veil",
        "Potion of the Howling Wind",
    };

    stat_type_e type = get_random_in_range(0, STAT_TYPE_NUM - 1);
    int max_increase = 0;
    switch ((int) type)
    {
        case HEALTH:
            max_increase = player->regen_limit * MAX_PERCENT_FOOD_REGEN_FROM_HEALTH / 100;
            break;
        case AGILITY:
            max_increase = player->base_stats.agility * MAX_PERCENT_AGILITY_INCREASE / 100;
            break;
        case STRENGTH:
            max_increase = player->base_stats.strength * MAX_PERCENT_STRENGTH_INCREASE / 100;
            break;
    }
    elixir->stat = type;
    elixir->increase = get_random_in_range(1, max_increase);
    elixir->duration = get_random_in_range(MIN_ELIXIR_DURATION_SECONDS, MAX_ELIXIR_DURATION_SECONDS);

    int name_pos = get_random_in_range(0, CONSUMABLES_TYPE_MAX_NUM - 1);
    strcpy(elixir->name, names[name_pos]);
}

void generate_elixir(room_t *room, const player_t *player)
{
    int count_elixirs = room->consumables.elixir_num;
    object_t *coords = &room->consumables.elixirs[count_elixirs].geometry;
    do
    {
        generate_coords_of_entity(room, coords);
    }
    while (!check_unoccupied_room(coords, room));
    generate_elixir_data(&room->consumables.elixirs[count_elixirs].elixir, player);
    room->consumables.elixir_num++;
}

void generate_scroll_data(scroll_t *scroll, const player_t *player)
{
    static const char *names[CONSUMABLES_TYPE_MAX_NUM] = {
        "Scroll of Shadowstep",
        "Parchment of Eternal Flame",
        "Manuscript of Forgotten Truths",
        "Scroll of Iron Will",
        "Vellum of the Void",
        "Scroll of Whispers",
        "Tome of the Lost King",
        "Scroll of Unseen Paths",
        "Parchment of Thunderous Roar",
    };

    stat_type_e type = rand() % STAT_TYPE_NUM;
    int max_increase = 0;
    switch ((int) type)
    {
        case HEALTH:
            max_increase = player->regen_limit * MAX_PERCENT_FOOD_REGEN_FROM_HEALTH / 100;
            break;
        case AGILITY:
            max_increase = player->base_stats.agility * MAX_PERCENT_AGILITY_INCREASE / 100;
            break;
        case STRENGTH:
            max_increase = player->base_stats.strength * MAX_PERCENT_STRENGTH_INCREASE / 100;
            break;
    }
    scroll->stat = type;
    scroll->increase = get_random_in_range(1, max_increase);

    int name_pos = get_random_in_range(0, CONSUMABLES_TYPE_MAX_NUM - 1);
    strcpy(scroll->name, names[name_pos]);
}

void generate_scroll(room_t *room, const player_t *player)
{
    int count_scrolls = room->consumables.scroll_num;
    object_t *coords = &room->consumables.scrolls[count_scrolls].geometry;
    do
    {
        generate_coords_of_entity(room, coords);
    }
    while (!check_unoccupied_room(coords, room));
    generate_scroll_data(&room->consumables.scrolls[count_scrolls].scroll, player);
    room->consumables.scroll_num++;
}

void generate_weapon_data(weapon_t *weapon, const player_t *player)
{
    static const char *names[CONSUMABLES_TYPE_MAX_NUM] = {
        "Blade of the Forgotten Dawn",
        "Obsidian Reaver",
        "Fang of the Shadow Wolf",
        "Ironclad Cleaver",
        "Crimson Talon",
        "Thunderstrike Maul",
        "Serpent's Kiss Dagger",
        "Voidrend Sword",
        "Ebonheart Spear",
    };

    int max_strength = MAX_WEAPON_STRENGTH;
    if (player->weapon.strength < max_strength)
        max_strength = player->weapon.strength;
    weapon->strength = get_random_in_range(MIN_WEAPON_STRENGTH, max_strength);

    int name_pos = get_random_in_range(0, CONSUMABLES_TYPE_MAX_NUM - 1);
    strcpy(weapon->name, names[name_pos]);
}

void generate_weapon(room_t *room, const player_t *player)
{
    int count_weapons = room->consumables.weapon_num;
    object_t *coords = &room->consumables.weapons[count_weapons].geometry;
    do
    {
        generate_coords_of_entity(room, coords);
    }
    while (!check_unoccupied_room(coords, room));
    generate_weapon_data(&room->consumables.weapons[count_weapons].weapon, player);
    room->consumables.weapon_num++;
}

void generate_consumables(level_t *level, int player_room, const player_t *player, int level_num)
{
    generate_consumable_func generate_consumable[CONSUMABLES_TYPES_NUM] = {
        generate_food,
        generate_elixir,
        generate_scroll,
        generate_weapon,
    };

    // Каждые LEVEL_UPDATE_DIFFICULTY количество расходников будет уменьшатся
    int max_consumables = MAX_CONSUMABLES_PER_ROOM - level_num / LEVEL_UPDATE_DIFFICULTY;
    for (int room = 0; room < ROOMS_NUM; room++)
    {
        if (room == player_room)
            continue;
        
        int count_consumables = get_random_in_range(0, max_consumables);
        for (int i = 0; i < count_consumables; i++)
        {
            int type_consumable = get_random_in_range(0, CONSUMABLES_TYPES_NUM - 1);
            generate_consumable[type_consumable](&level->rooms[room], player);
        }
    }
}
