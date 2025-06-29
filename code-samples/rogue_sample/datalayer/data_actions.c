#include "data_actions.h"

#define MAX_JSON_SIZE 65536

static void load_json_coords(object_t *entity_coords, struct json_object *coords)
{
    struct json_object *coordinates;
    struct json_object *size;

    json_object_object_get_ex(coords, "coordinates", &coordinates);
    json_object_object_get_ex(coords, "size", &size);

    for (size_t i = 0; i < COORDS_NUM; i++)
    {
        struct json_object *cur_coord = json_object_array_get_idx(coordinates, i);
        struct json_object *cur_size = json_object_array_get_idx(size, i);

        entity_coords->coordinates[i] = json_object_get_int(cur_coord);
        entity_coords->size[i] = json_object_get_int(cur_size);
    }
}

static void load_json_base_stats(character_t *base_stats, struct json_object *stats)
{
    struct json_object *health;
    struct json_object *strength;
    struct json_object *agility;
    struct json_object *coords;

    json_object_object_get_ex(stats, "health", &health);
    json_object_object_get_ex(stats, "strength", &strength);
    json_object_object_get_ex(stats, "agility", &agility);
    json_object_object_get_ex(stats, "coords", &coords);

    base_stats->health = json_object_get_int(health);
    base_stats->strength = json_object_get_int(strength);
    base_stats->agility = json_object_get_int(agility);
    load_json_coords(&base_stats->coords, coords);
}

static void load_json_backpack(player_t *player, struct json_object *backpack_data)
{
    struct json_object *foods, *foods_regen, *foods_name;
    struct json_object *elixirs, *elixirs_type, *elixirs_value, *elixirs_duration, *elixirs_name;
    struct json_object *scrolls, *scrolls_type, *scrolls_value, *scrolls_name;
    struct json_object *weapons, *weapons_strength, *weapons_name;
    struct json_object *treasures;

    json_object_object_get_ex(backpack_data, "foods", &foods);
    json_object_object_get_ex(foods, "foodRegen", &foods_regen);
    json_object_object_get_ex(foods, "foodName", &foods_name);
    player->backpack.food_num = json_object_array_length(foods_regen);
    for (size_t i = 0; i < player->backpack.food_num; i++)
    {
        struct json_object *regen = json_object_array_get_idx(foods_regen, i);
        struct json_object *name = json_object_array_get_idx(foods_name, i);
        player->backpack.foods[i].to_regen = json_object_get_int(regen);
        strcpy(player->backpack.foods[i].name, json_object_get_string(name));
    }

    json_object_object_get_ex(backpack_data, "elixirs", &elixirs);
    json_object_object_get_ex(elixirs, "elixirType", &elixirs_type);
    json_object_object_get_ex(elixirs, "elixirValue", &elixirs_value);
    json_object_object_get_ex(elixirs, "elixirDuration", &elixirs_duration);
    json_object_object_get_ex(elixirs, "elixirName", &elixirs_name);
    player->backpack.elixir_num = json_object_array_length(elixirs_type);
    for (size_t i = 0; i < player->backpack.elixir_num; i++)
    {
        struct json_object *type = json_object_array_get_idx(elixirs_type, i);
        struct json_object *value = json_object_array_get_idx(elixirs_value, i);
        struct json_object *duration = json_object_array_get_idx(elixirs_duration, i);
        struct json_object *name = json_object_array_get_idx(elixirs_name, i);
        player->backpack.elixirs[i].stat = json_object_get_int(type);
        player->backpack.elixirs[i].increase = json_object_get_int(value);
        player->backpack.elixirs[i].duration = json_object_get_int(duration);
        strcpy(player->backpack.elixirs[i].name, json_object_get_string(name));
    }

    json_object_object_get_ex(backpack_data, "scrolls", &scrolls);
    json_object_object_get_ex(scrolls, "scrollType", &scrolls_type);
    json_object_object_get_ex(scrolls, "scrollValue", &scrolls_value);
    json_object_object_get_ex(scrolls, "scrollName", &scrolls_name);
    player->backpack.scroll_num = json_object_array_length(scrolls_type);
    for (size_t i = 0; i < player->backpack.scroll_num; i++)
    {
        struct json_object *type = json_object_array_get_idx(scrolls_type, i);
        struct json_object *value = json_object_array_get_idx(scrolls_value, i);
        struct json_object *name = json_object_array_get_idx(scrolls_name, i);
        player->backpack.scrolls[i].stat = json_object_get_int(type);
        player->backpack.scrolls[i].increase = json_object_get_int(value);
        strcpy(player->backpack.scrolls[i].name, json_object_get_string(name));
    }

    json_object_object_get_ex(backpack_data, "weapons", &weapons);
    json_object_object_get_ex(weapons, "weaponStrength", &weapons_strength);
    json_object_object_get_ex(weapons, "weaponName", &weapons_name);
    player->backpack.weapon_num = json_object_array_length(weapons_strength);
    for (size_t i = 0; i < player->backpack.weapon_num; i++)
    {
        struct json_object *strength = json_object_array_get_idx(weapons_strength, i);
        struct json_object *name = json_object_array_get_idx(weapons_name, i);
        player->backpack.weapons[i].strength = json_object_get_int(strength);
        strcpy(player->backpack.weapons[i].name, json_object_get_string(name));
    }

    json_object_object_get_ex(backpack_data, "treasures", &treasures);
    player->backpack.treasures.value = json_object_get_int(treasures);

    player->backpack.current_size = player->backpack.elixir_num + player->backpack.food_num + \
                                        player->backpack.scroll_num + player->backpack.weapon_num;
}

static void load_json_buffs(player_t *player, struct json_object *buffs)
{
    struct json_object *max_health, *max_health_increase, *max_health_duration;
    struct json_object *agility, *agility_increase, *agility_duration;
    struct json_object *strength, *strength_increase, *strength_duration;

    json_object_object_get_ex(buffs, "max_health", &max_health);
    json_object_object_get_ex(max_health, "increase", &max_health_increase);
    json_object_object_get_ex(max_health, "duration", &max_health_duration);
    player->elixir_buffs.current_health_buff_num = json_object_array_length(max_health_increase);
    for (size_t i = 0; i < player->elixir_buffs.current_health_buff_num; i++)
    {
        struct json_object *increase = json_object_array_get_idx(max_health_increase, i);
        struct json_object *duration = json_object_array_get_idx(max_health_duration, i);
        player->elixir_buffs.max_health[i].stat_increase = json_object_get_int(increase);
        player->elixir_buffs.max_health[i].effect_end = json_object_get_int(duration);
    }

    json_object_object_get_ex(buffs, "agility", &agility);
    json_object_object_get_ex(agility, "increase", &agility_increase);
    json_object_object_get_ex(agility, "duration", &agility_duration);
    player->elixir_buffs.current_agility_buff_num = json_object_array_length(agility_increase);
    for (size_t i = 0; i < player->elixir_buffs.current_agility_buff_num; i++)
    {
        struct json_object *increase = json_object_array_get_idx(agility_increase, i);
        struct json_object *duration = json_object_array_get_idx(agility_duration, i);
        player->elixir_buffs.agility[i].stat_increase = json_object_get_int(increase);
        player->elixir_buffs.agility[i].effect_end = json_object_get_int(duration);
    }

    json_object_object_get_ex(buffs, "strength", &strength);
    json_object_object_get_ex(strength, "increase", &strength_increase);
    json_object_object_get_ex(strength, "duration", &strength_duration);
    player->elixir_buffs.current_strength_buff_num = json_object_array_length(strength_increase);
    for (size_t i = 0; i < player->elixir_buffs.current_strength_buff_num; i++)
    {
        struct json_object *increase = json_object_array_get_idx(strength_increase, i);
        struct json_object *duration = json_object_array_get_idx(strength_duration, i);
        player->elixir_buffs.strength[i].stat_increase = json_object_get_int(increase);
        player->elixir_buffs.strength[i].effect_end = json_object_get_int(duration);
    }
}

static void load_json_player_data(player_t *player, struct json_object *player_data)
{
    struct json_object *base_stats;
    struct json_object *max_hp;
    struct json_object *backpack;
    struct json_object *weapon;
    struct json_object *buffs;

    json_object_object_get_ex(player_data, "baseStats", &base_stats);
    json_object_object_get_ex(player_data, "maxHP", &max_hp);
    json_object_object_get_ex(player_data, "backpack", &backpack);
    json_object_object_get_ex(player_data, "weaponStrength", &weapon);
    json_object_object_get_ex(player_data, "buffs", &buffs);

    load_json_base_stats(&player->base_stats, base_stats);
    player->regen_limit = json_object_get_int(max_hp);
    load_json_backpack(player, backpack);
    player->weapon.strength = json_object_get_int(weapon);
    load_json_buffs(player, buffs);
}

static void load_json_consumables(consumables_room_t *room, struct json_object *consum_data)
{
    struct json_object *room_food, *to_regen, *geometry_food, *name_food;
    struct json_object *room_elixirs, *elixirs_type, *elixirs_value, *elixirs_duration, *geometry_elixirs, *name_elixirs;
    struct json_object *room_scrolls, *scrolls_type, *scrolls_value, *geometry_scrolls, *name_scrolls;
    struct json_object *room_weapons, *strength, *geometry_weapon, *name_weapon;

    json_object_object_get_ex(consum_data, "roomFood", &room_food);
    json_object_object_get_ex(room_food, "regen", &to_regen);
    json_object_object_get_ex(room_food, "geometry", &geometry_food);
    json_object_object_get_ex(room_food, "name", &name_food);
    room->food_num = json_object_array_length(to_regen);
    for (size_t i = 0; i < room->food_num; i++)
    {
        struct json_object *coords = json_object_array_get_idx(geometry_food, i);
        struct json_object *regen = json_object_array_get_idx(to_regen, i);
        struct json_object *name = json_object_array_get_idx(name_food, i);

        load_json_coords(&room->room_food[i].geometry, coords);
        room->room_food[i].food.to_regen = json_object_get_int(regen);
        strcpy(room->room_food[i].food.name, json_object_get_string(name));
    }

    json_object_object_get_ex(consum_data, "roomElixirs", &room_elixirs);
    json_object_object_get_ex(room_elixirs, "elixirType", &elixirs_type);
    json_object_object_get_ex(room_elixirs, "elixirValue", &elixirs_value);
    json_object_object_get_ex(room_elixirs, "elixirDuration", &elixirs_duration);
    json_object_object_get_ex(room_elixirs, "geometry", &geometry_elixirs);
    json_object_object_get_ex(room_elixirs, "name", &name_elixirs);
    room->elixir_num = json_object_array_length(elixirs_type);
    for (size_t i = 0; i < room->elixir_num; i++)
    {
        struct json_object *coords = json_object_array_get_idx(geometry_elixirs, i);
        struct json_object *type = json_object_array_get_idx(elixirs_type, i);
        struct json_object *value = json_object_array_get_idx(elixirs_value, i);
        struct json_object *duration = json_object_array_get_idx(elixirs_duration, i);
        struct json_object *name = json_object_array_get_idx(name_elixirs, i);

        load_json_coords(&room->elixirs[i].geometry, coords);
        room->elixirs[i].elixir.stat = json_object_get_int(type);
        room->elixirs[i].elixir.increase = json_object_get_int(value);
        room->elixirs[i].elixir.duration = json_object_get_int(duration);
        strcpy(room->elixirs[i].elixir.name, json_object_get_string(name));
    }

    json_object_object_get_ex(consum_data, "roomScrolls", &room_scrolls);
    json_object_object_get_ex(room_scrolls, "scrollType", &scrolls_type);
    json_object_object_get_ex(room_scrolls, "scrollValue", &scrolls_value);
    json_object_object_get_ex(room_scrolls, "geometry", &geometry_scrolls);
    json_object_object_get_ex(room_scrolls, "name", &name_scrolls);
    room->scroll_num = json_object_array_length(scrolls_type);
    for (size_t i = 0; i < room->scroll_num; i++)
    {
        struct json_object *coords = json_object_array_get_idx(geometry_scrolls, i);
        struct json_object *type = json_object_array_get_idx(scrolls_type, i);
        struct json_object *value = json_object_array_get_idx(scrolls_value, i);
        struct json_object *name = json_object_array_get_idx(name_scrolls, i);

        load_json_coords(&room->scrolls[i].geometry, coords);
        room->scrolls[i].scroll.stat = json_object_get_int(type);
        room->scrolls[i].scroll.increase = json_object_get_int(value);
        strcpy(room->scrolls[i].scroll.name, json_object_get_string(name));
    }

    json_object_object_get_ex(consum_data, "roomWeapon", &room_weapons);
    json_object_object_get_ex(room_weapons, "strength", &strength);
    json_object_object_get_ex(room_weapons, "geometry", &geometry_weapon);
    json_object_object_get_ex(room_weapons, "name", &name_weapon);
    room->weapon_num = json_object_array_length(strength);
    for (size_t i = 0; i < room->weapon_num; i++)
    {
        struct json_object *coords = json_object_array_get_idx(geometry_weapon, i);
        struct json_object *strength_value = json_object_array_get_idx(strength, i);
        struct json_object *name = json_object_array_get_idx(name_weapon, i);

        load_json_coords(&room->weapons[i].geometry, coords);
        room->weapons[i].weapon.strength = json_object_get_int(strength_value);
        strcpy(room->weapons[i].weapon.name, json_object_get_string(name));
    }
}

static void load_json_level_rooms(level_t *level, struct json_object *rooms_data)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        struct json_object *room = json_object_array_get_idx(rooms_data, i);
        struct json_object *coords;
        struct json_object *consumables;
        struct json_object *monsters;

        json_object_object_get_ex(room, "coords", &coords);
        json_object_object_get_ex(room, "consumablesRoom", &consumables);
        json_object_object_get_ex(room, "monsters", &monsters);

        load_json_coords(&level->rooms[i].coords, coords);
        load_json_consumables(&level->rooms[i].consumables, consumables);

        level->rooms[i].monster_num = json_object_array_length(monsters);
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
        {
            struct json_object *monster_data = json_object_array_get_idx(monsters, j);

            struct json_object *base_stats;
            struct json_object *type;
            struct json_object *hostility;
            struct json_object *chasing;
            struct json_object *direction;

            json_object_object_get_ex(monster_data, "baseStats", &base_stats);
            json_object_object_get_ex(monster_data, "type", &type);
            json_object_object_get_ex(monster_data, "hostility", &hostility);
            json_object_object_get_ex(monster_data, "chasing", &chasing);
            json_object_object_get_ex(monster_data, "direction", &direction);

            load_json_base_stats(&level->rooms[i].monsters[j].base_stats, base_stats);
            level->rooms[i].monsters[j].type = json_object_get_int(type);
            level->rooms[i].monsters[j].hostility = json_object_get_int(hostility);
            level->rooms[i].monsters[j].is_chasing = json_object_get_boolean(chasing);
            level->rooms[i].monsters[j].dir = json_object_get_int(direction);
        }
    }
}

static void load_json_level_passages(level_t *level, struct json_object *passages_data)
{
    level->passages.passages_num = json_object_array_length(passages_data);
    for (size_t i = 0; i < level->passages.passages_num; i++)
    {
        struct json_object *passage = json_object_array_get_idx(passages_data, i);
        load_json_coords(level->passages.passages + i, passage);
    }
}

static void load_json_level_data(level_t *level, struct json_object *level_data)
{
    struct json_object *coords;
    struct json_object *rooms;
    struct json_object *passages;
    struct json_object *level_num;
    struct json_object *end_of_level;

    json_object_object_get_ex(level_data, "coords", &coords);
    json_object_object_get_ex(level_data, "rooms", &rooms);
    json_object_object_get_ex(level_data, "passages", &passages);
    json_object_object_get_ex(level_data, "levelNum", &level_num);
    json_object_object_get_ex(level_data, "endOfLevel", &end_of_level);

    load_json_coords(&level->coords, coords);
    load_json_level_rooms(level, rooms);
    load_json_level_passages(level, passages);
    level->level_num = json_object_get_int(level_num);
    load_json_coords(&level->end_of_level, end_of_level);
}

static void load_json_map_data(map_t *map, struct json_object *map_data)
{
    struct json_object *visible_rooms;
    struct json_object *visible_passages;

    json_object_object_get_ex(map_data, "visibleRooms", &visible_rooms);
    json_object_object_get_ex(map_data, "visiblePassages", &visible_passages);

    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        struct json_object *visible = json_object_array_get_idx(visible_rooms, i);
        map->visible_rooms[i] = json_object_get_boolean(visible);
    }

    for (size_t i = 0; i < MAX_PASSAGES_NUM; i++)
    {
        struct json_object *visible = json_object_array_get_idx(visible_passages, i);
        map->visible_passages[i] = json_object_get_boolean(visible);
    }
}

char *read_file_to_str(const char *filename)
{
    FILE *data_file = fopen(filename, "r");
    char *str = malloc(MAX_JSON_SIZE), *buffer = malloc(MAX_JSON_SIZE);
    if (fgets(str, MAX_JSON_SIZE, data_file) != NULL)
        while (fgets(buffer, MAX_JSON_SIZE, data_file) != NULL)
            strcat(str, buffer);

    free(buffer);
    fclose(data_file);
    return str;
}

void load_data(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *save_path, const char *stat_path)
{
    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
        battles[i].is_fight = false;

    char *buffer = read_file_to_str(save_path);
    struct json_object *parsed_json = json_tokener_parse(buffer);

    struct json_object *player_data;
    struct json_object *level_data;
    struct json_object *map_data;
    
    int success = 0;
    success += json_object_object_get_ex(parsed_json, "player", &player_data);
    success += json_object_object_get_ex(parsed_json, "level", &level_data);
    success += json_object_object_get_ex(parsed_json, "map", &map_data);

    if (success == 3)
    {
        load_json_player_data(player, player_data);
        load_json_level_data(level, level_data);
        load_json_map_data(map, map_data);
    }
    else
        init_level(player, level, map, battles, stat_path);

    free(buffer);
    json_object_put(parsed_json);
}

static struct json_object *get_json_coords(const object_t *entity_coords)
{
    struct json_object *coords = json_object_new_object();
    struct json_object *coordinates = json_object_new_array();
    struct json_object *size = json_object_new_array();
    for (size_t i = 0; i < COORDS_NUM; i++)
    {
        json_object_array_add(coordinates, json_object_new_int(entity_coords->coordinates[i]));
        json_object_array_add(size, json_object_new_int(entity_coords->size[i]));
    }

    json_object_object_add(coords, "coordinates", coordinates);
    json_object_object_add(coords, "size", size);

    return coords;
}

static struct json_object *get_json_base_stats(const character_t *character)
{
    struct json_object *base_stats = json_object_new_object();
    json_object_object_add(base_stats, "health", json_object_new_int(character->health));
    json_object_object_add(base_stats, "strength", json_object_new_int(character->strength));
    json_object_object_add(base_stats, "agility", json_object_new_int(character->agility));
    json_object_object_add(base_stats, "coords", get_json_coords(&character->coords));
    return base_stats;
}

static struct json_object *get_json_player_backpack(const player_t *player)
{
    struct json_object *backpack = json_object_new_object();

    struct json_object *foods_to_regen = json_object_new_array();
    struct json_object *foods_name = json_object_new_array();
    for (size_t i = 0; i < player->backpack.food_num; i++)
    {
        struct json_object *food = json_object_new_int(player->backpack.foods[i].to_regen);
        struct json_object *name = json_object_new_string(player->backpack.foods[i].name);
        json_object_array_add(foods_to_regen, food);
        json_object_array_add(foods_name, name);
    }
    struct json_object *foods = json_object_new_object();
    json_object_object_add(foods, "foodRegen", foods_to_regen);
    json_object_object_add(foods, "foodName", foods_name);

    struct json_object *elixirs_type = json_object_new_array();
    struct json_object *elixirs_value = json_object_new_array();
    struct json_object *elixirs_duration= json_object_new_array();
    struct json_object *elixirs_name = json_object_new_array();
    for (size_t i = 0; i < player->backpack.elixir_num; i++)
    {
        struct json_object *type = json_object_new_int(player->backpack.elixirs[i].stat);
        struct json_object *value = json_object_new_int(player->backpack.elixirs[i].increase);
        struct json_object *duration = json_object_new_int(player->backpack.elixirs[i].duration);
        struct json_object *name = json_object_new_string(player->backpack.elixirs[i].name);
        json_object_array_add(elixirs_type, type);
        json_object_array_add(elixirs_value, value);
        json_object_array_add(elixirs_duration, duration);
        json_object_array_add(elixirs_name, name);
    }
    struct json_object *elixirs = json_object_new_object();
    json_object_object_add(elixirs, "elixirType", elixirs_type);
    json_object_object_add(elixirs, "elixirValue", elixirs_value);
    json_object_object_add(elixirs, "elixirDuration", elixirs_duration);
    json_object_object_add(elixirs, "elixirName", elixirs_name);

    struct json_object *scrolls_type = json_object_new_array();
    struct json_object *scrolls_value = json_object_new_array();
    struct json_object *scrolls_name = json_object_new_array();
    for (size_t i = 0; i < player->backpack.scroll_num; i++)
    {
        struct json_object *type = json_object_new_int(player->backpack.scrolls[i].stat);
        struct json_object *value = json_object_new_int(player->backpack.scrolls[i].increase);
        struct json_object *name = json_object_new_string(player->backpack.scrolls[i].name);
        json_object_array_add(scrolls_type, type);
        json_object_array_add(scrolls_value, value);
        json_object_array_add(scrolls_name, name);
    }
    struct json_object *scrolls = json_object_new_object();
    json_object_object_add(scrolls, "scrollType", scrolls_type);
    json_object_object_add(scrolls, "scrollValue", scrolls_value);
    json_object_object_add(scrolls, "scrollName", scrolls_name);

    struct json_object *weapons_strength = json_object_new_array();
    struct json_object *weapons_name = json_object_new_array();
    for (size_t i = 0; i < player->backpack.weapon_num; i++)
    {
        struct json_object *weapon = json_object_new_int(player->backpack.weapons[i].strength);
        struct json_object *name = json_object_new_string(player->backpack.weapons[i].name);
        json_object_array_add(weapons_strength, weapon);
        json_object_array_add(weapons_name, name);
    }
    struct json_object *weapons = json_object_new_object();
    json_object_object_add(weapons, "weaponStrength", weapons_strength);
    json_object_object_add(weapons, "weaponName", weapons_name);

    json_object_object_add(backpack, "foods", foods);
    json_object_object_add(backpack, "elixirs", elixirs);
    json_object_object_add(backpack, "scrolls", scrolls);
    json_object_object_add(backpack, "weapons", weapons);
    json_object_object_add(backpack, "treasures", json_object_new_int(player->backpack.treasures.value));

    return backpack;
}

static struct json_object *get_json_player_buffs(const player_t *player)
{
    struct json_object *buffs = json_object_new_object();

    struct json_object *max_health = json_object_new_object();
    struct json_object *max_health_increase = json_object_new_array();
    struct json_object *max_health_duration = json_object_new_array();
    for (size_t i = 0; i < player->elixir_buffs.current_health_buff_num; i++)
    {
        struct json_object *increase = json_object_new_int(player->elixir_buffs.max_health[i].stat_increase);
        struct json_object *duration = json_object_new_int(player->elixir_buffs.max_health[i].effect_end);
        json_object_array_add(max_health_increase, increase);
        json_object_array_add(max_health_duration, duration);
    }
    json_object_object_add(max_health, "increase", max_health_increase);
    json_object_object_add(max_health, "duration", max_health_duration);

    struct json_object *agility = json_object_new_object();
    struct json_object *agility_increase = json_object_new_array();
    struct json_object *agility_duration = json_object_new_array();
    for (size_t i = 0; i < player->elixir_buffs.current_agility_buff_num; i++)
    {
        struct json_object *increase = json_object_new_int(player->elixir_buffs.agility[i].stat_increase);
        struct json_object *duration = json_object_new_int(player->elixir_buffs.agility[i].effect_end);
        json_object_array_add(agility_increase, increase);
        json_object_array_add(agility_duration, duration);
    }
    json_object_object_add(agility, "increase", agility_increase);
    json_object_object_add(agility, "duration", agility_duration);

    struct json_object *strength = json_object_new_object();
    struct json_object *strength_increase = json_object_new_array();
    struct json_object *strength_duration = json_object_new_array();
    for (size_t i = 0; i < player->elixir_buffs.current_strength_buff_num; i++)
    {
        struct json_object *increase = json_object_new_int(player->elixir_buffs.strength[i].stat_increase);
        struct json_object *duration = json_object_new_int(player->elixir_buffs.strength[i].effect_end);
        json_object_array_add(strength_increase, increase);
        json_object_array_add(strength_duration, duration);
    }
    json_object_object_add(strength, "increase", strength_increase);
    json_object_object_add(strength, "duration", strength_duration);

    json_object_object_add(buffs, "max_health", max_health);
    json_object_object_add(buffs, "agility", agility);
    json_object_object_add(buffs, "strength", strength);

    return buffs;
}

static struct json_object *json_player_data(const player_t *player)
{
    struct json_object *player_info = json_object_new_object();
    json_object_object_add(player_info, "baseStats", get_json_base_stats(&player->base_stats));
    json_object_object_add(player_info, "maxHP", json_object_new_int(player->regen_limit));
    json_object_object_add(player_info, "backpack", get_json_player_backpack(player));
    json_object_object_add(player_info, "weaponStrength", json_object_new_int(player->weapon.strength));
    json_object_object_add(player_info, "buffs", get_json_player_buffs(player));
    return player_info;
}

static struct json_object *get_json_consumables(const consumables_room_t *room)
{
    struct json_object *consum = json_object_new_object();
    
    struct json_object *room_food = json_object_new_object();
    struct json_object *to_regen = json_object_new_array();
    struct json_object *geometry_food = json_object_new_array();
    struct json_object *name_food = json_object_new_array();
    for (size_t i = 0; i < room->food_num; i++)
    {
        json_object_array_add(to_regen, json_object_new_int(room->room_food[i].food.to_regen));
        json_object_array_add(geometry_food, get_json_coords(&room->room_food[i].geometry));
        json_object_array_add(name_food, json_object_new_string(room->room_food[i].food.name));
    }
    json_object_object_add(room_food, "regen", to_regen);
    json_object_object_add(room_food, "geometry", geometry_food);
    json_object_object_add(room_food, "name", name_food);

    struct json_object *room_elixirs = json_object_new_object();
    struct json_object *elixirs_type = json_object_new_array();
    struct json_object *elixirs_value = json_object_new_array();
    struct json_object *elixirs_duration= json_object_new_array();
    struct json_object *geometry_elixirs = json_object_new_array();
    struct json_object *name_elixirs = json_object_new_array();
    for (size_t i = 0; i < room->elixir_num; i++)
    {
        json_object_array_add(elixirs_type, json_object_new_int(room->elixirs[i].elixir.stat));
        json_object_array_add(elixirs_value, json_object_new_int(room->elixirs[i].elixir.increase));
        json_object_array_add(elixirs_duration, json_object_new_int(room->elixirs[i].elixir.duration));
        json_object_array_add(geometry_elixirs, get_json_coords(&room->elixirs[i].geometry));
        json_object_array_add(name_elixirs, json_object_new_string(room->elixirs[i].elixir.name));
    }
    json_object_object_add(room_elixirs, "elixirType", elixirs_type);
    json_object_object_add(room_elixirs, "elixirValue", elixirs_value);
    json_object_object_add(room_elixirs, "elixirDuration", elixirs_duration);
    json_object_object_add(room_elixirs, "geometry", geometry_elixirs);
    json_object_object_add(room_elixirs, "name", name_elixirs);

    struct json_object *room_scrolls = json_object_new_object();
    struct json_object *scrolls_type = json_object_new_array();
    struct json_object *scrolls_value = json_object_new_array();
    struct json_object *geometry_scrolls = json_object_new_array();
    struct json_object *name_scrolls = json_object_new_array();
    for (size_t i = 0; i < room->scroll_num; i++)
    {
        json_object_array_add(scrolls_type, json_object_new_int(room->scrolls[i].scroll.stat));
        json_object_array_add(scrolls_value, json_object_new_int(room->scrolls[i].scroll.increase));
        json_object_array_add(geometry_scrolls, get_json_coords(&room->scrolls[i].geometry));
        json_object_array_add(name_scrolls, json_object_new_string(room->scrolls[i].scroll.name));
    }
    json_object_object_add(room_scrolls, "scrollType", scrolls_type);
    json_object_object_add(room_scrolls, "scrollValue", scrolls_value);
    json_object_object_add(room_scrolls, "geometry", geometry_scrolls);
    json_object_object_add(room_scrolls, "name", name_scrolls);

    struct json_object *room_weapon = json_object_new_object();
    struct json_object *strength = json_object_new_array();
    struct json_object *geometry_weapon = json_object_new_array();
    struct json_object *name_weapon = json_object_new_array();
    for (size_t i = 0; i < room->weapon_num; i++)
    {
        json_object_array_add(strength, json_object_new_int(room->weapons[i].weapon.strength));
        json_object_array_add(geometry_weapon, get_json_coords(&room->weapons[i].geometry));
        json_object_array_add(name_weapon, json_object_new_string(room->weapons[i].weapon.name));
    }
    json_object_object_add(room_weapon, "strength", strength);
    json_object_object_add(room_weapon, "geometry", geometry_weapon);
    json_object_object_add(room_weapon, "name", name_weapon);

    json_object_object_add(consum, "roomFood", room_food);
    json_object_object_add(consum, "roomScrolls", room_scrolls);
    json_object_object_add(consum, "roomElixirs", room_elixirs);
    json_object_object_add(consum, "roomWeapon", room_weapon);

    return consum;
}

static struct json_object *get_json_level_rooms(const level_t *level)
{
    struct json_object *rooms = json_object_new_array();
    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        struct json_object *room = json_object_new_object();
        json_object_object_add(room, "coords", get_json_coords(&level->rooms[i].coords));
        json_object_object_add(room, "consumablesRoom", get_json_consumables(&level->rooms[i].consumables));

        struct json_object *monsters = json_object_new_array();
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
        {
            struct json_object *monster = json_object_new_object();
            json_object_object_add(monster, "baseStats", get_json_base_stats(&level->rooms[i].monsters[j].base_stats));
            json_object_object_add(monster, "type", json_object_new_int(level->rooms[i].monsters[j].type));
            json_object_object_add(monster, "hostility", json_object_new_int(level->rooms[i].monsters[j].hostility));
            json_object_object_add(monster, "chasing", json_object_new_boolean(level->rooms[i].monsters[j].is_chasing));
            json_object_object_add(monster, "direction", json_object_new_int(level->rooms[i].monsters[j].dir));
            json_object_array_add(monsters, monster);
        }
        json_object_object_add(room, "monsters", monsters);
        json_object_array_add(rooms, room);
    }
    return rooms;
}

static struct json_object *get_json_level_passages(const level_t *level)
{
    struct json_object *passages = json_object_new_array();
    for (size_t i = 0; i < level->passages.passages_num; i++)
    {
        struct json_object *passage = get_json_coords(&level->passages.passages[i]);
        json_object_array_add(passages, passage);
    }
    return passages;
}

static struct json_object *json_level_data(const level_t *level)
{
    struct json_object *level_info = json_object_new_object();
    json_object_object_add(level_info, "coords", get_json_coords(&level->coords));
    json_object_object_add(level_info, "rooms", get_json_level_rooms(level));
    json_object_object_add(level_info, "passages", get_json_level_passages(level));
    json_object_object_add(level_info, "levelNum", json_object_new_int(level->level_num));
    json_object_object_add(level_info, "endOfLevel", get_json_coords(&level->end_of_level));
    return level_info;
}

static struct json_object *json_map_data(const map_t *map)
{
    struct json_object *map_data = json_object_new_object();
    
    struct json_object *visible_rooms = json_object_new_array();
    for (size_t i = 0; i < ROOMS_NUM; i++)
        json_object_array_add(visible_rooms, json_object_new_boolean(map->visible_rooms[i]));
    json_object_object_add(map_data, "visibleRooms", visible_rooms);

    struct json_object *visible_passages = json_object_new_array();
    for (size_t i = 0; i < MAX_PASSAGES_NUM; i++)
        json_object_array_add(visible_passages, json_object_new_boolean(map->visible_passages[i]));
    json_object_object_add(map_data, "visiblePassages", visible_passages);

    return map_data;
}

void save_data(const player_t *player, const level_t *level, const map_t *map, const char *filename)
{
    struct json_object *data = json_object_new_object();
    struct json_object *player_data = json_player_data(player);
    struct json_object *level_data = json_level_data(level);
    struct json_object *map_data = json_map_data(map);

    json_object_object_add(data, "player", player_data);
    json_object_object_add(data, "level", level_data);
    json_object_object_add(data, "map", map_data);

    FILE *save;
    save = fopen(filename, "w");
    fprintf(save, "%s", json_object_to_json_string_ext(data, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    fclose(save);
    json_object_put(data);
}

void init_level(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *stat_path)
{
    *player = (player_t) {
        .base_stats.health = 500,
        .base_stats.agility = 70,
        .base_stats.strength = 70,
        .regen_limit = 500,
        .backpack.current_size = 0,
        .backpack.elixir_num = 0,
        .backpack.food_num = 0,
        .backpack.scroll_num = 0,
        .backpack.weapon_num = 0,
        .backpack.treasures.value = 0,
        .weapon.strength = 0,
        .elixir_buffs.current_agility_buff_num = 0,
        .elixir_buffs.current_health_buff_num = 0,
        .elixir_buffs.current_strength_buff_num = 0,
    };

    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
        battles[i].is_fight = false;
    
    level->level_num = 0;
    generate_next_level(level, player);
    clear_map_data(map);

    session_stat stat = {
        .treasures = 0,
        .level = 0,
        .enemies = 0,
        .food = 0,
        .elixirs = 0,
        .scrolls = 0,
        .attacks = 0,
        .missed = 0,
        .moves = 0,
    };
    save_session_stat(&stat, stat_path);
}

void get_standart_save(const char *save_path, const char *stat_path)
{
    player_t player;
    level_t level;
    map_t map;
    battle_info_t battles[MAXIMUM_FIGHTS];
    init_level(&player, &level, &map, battles, stat_path);
    save_data(&player, &level, &map, save_path);
}
