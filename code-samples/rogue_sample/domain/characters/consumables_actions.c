#include "consumables_actions.h"

bool equal_weapons(const weapon_t *first, const weapon_t *second)
{
    return strcmp(first->name, second->name) == 0 && first->strength == second->strength;
}

void check_consumable(player_t *player, room_t *room)
{
    bool was_consumable = false;
    for (size_t i = 0; i < room->consumables.elixir_num && !was_consumable && player->backpack.elixir_num < CONSUMABLES_TYPE_MAX_NUM; i++)
        if (check_equal_coords(room->consumables.elixirs[i].geometry.coordinates, player->base_stats.coords.coordinates))
        {
            take_elixir(&player->backpack, room, room->consumables.elixirs + i);
            player->backpack.current_size++;
            was_consumable = true;
        }

    for (size_t i = 0; i < room->consumables.scroll_num && !was_consumable && player->backpack.scroll_num < CONSUMABLES_TYPE_MAX_NUM; i++)
        if (check_equal_coords(room->consumables.scrolls[i].geometry.coordinates, player->base_stats.coords.coordinates))
        {
            take_scroll(&player->backpack, room, room->consumables.scrolls + i);
            player->backpack.current_size++;
            was_consumable = true;
        }

    for (size_t i = 0; i < room->consumables.food_num && !was_consumable && player->backpack.food_num < CONSUMABLES_TYPE_MAX_NUM; i++)
        if (check_equal_coords(room->consumables.room_food[i].geometry.coordinates, player->base_stats.coords.coordinates))
        {
            take_food(&player->backpack, room, room->consumables.room_food + i);
            player->backpack.current_size++;
            was_consumable = true;
        }
    
    for (size_t i = 0; i < room->consumables.weapon_num && !was_consumable && player->backpack.weapon_num < CONSUMABLES_TYPE_MAX_NUM; i++)
        if (check_equal_coords(room->consumables.weapons[i].geometry.coordinates, player->base_stats.coords.coordinates))
        {
            take_weapon(player, room, room->consumables.weapons + i);
            player->backpack.current_size++;
            was_consumable = true;
        }
}

void take_scroll(backpack_t *players_backpack, room_t *current_room, const scroll_room_t *scroll)
{
    delete_from_room(current_room, &scroll->geometry, SCROLL);
    players_backpack->scrolls[players_backpack->scroll_num] = scroll->scroll;
    players_backpack->scroll_num++;
}

void take_elixir(backpack_t *players_backpack, room_t *current_room, const elixir_room_t *elixir)
{
    delete_from_room(current_room, &elixir->geometry, ELIXIR);
    players_backpack->elixirs[players_backpack->elixir_num] = elixir->elixir;
    players_backpack->elixir_num++;
}

void take_food(backpack_t *players_backpack, room_t *current_room, const food_room_t *food)
{
    delete_from_room(current_room, &food->geometry, FOOD);
    players_backpack->foods[players_backpack->food_num] = food->food;
    players_backpack->food_num++;
}

void take_weapon(player_t *player, room_t *current_room, const weapon_room_t *weapon)
{
    delete_from_room(current_room, &weapon->geometry, WEAPON);
    player->backpack.weapons[player->backpack.weapon_num] = weapon->weapon;
    player->backpack.weapon_num++;
}

void delete_from_room(room_t *room, const object_t *consumable_coords, consumable_types_e type)
{
    switch (type)
    {
    case ELIXIR:
    {
        size_t i = 0;
        for (; i < room->consumables.elixir_num && !check_equal_coords(consumable_coords->coordinates, room->consumables.elixirs[i].geometry.coordinates); i++);
        if (i != room->consumables.elixir_num)
        {
            room->consumables.elixirs[i] = room->consumables.elixirs[room->consumables.elixir_num - 1];
            room->consumables.elixir_num--;
        }
        break;
    }

    case FOOD:
    {
        size_t i = 0;
        for (; i < room->consumables.food_num && !check_equal_coords(consumable_coords->coordinates, room->consumables.room_food[i].geometry.coordinates); i++);
        if (i != room->consumables.food_num)
        {
            room->consumables.room_food[i] = room->consumables.room_food[room->consumables.food_num - 1];
            room->consumables.food_num--;
        }
        break;
    }

    case SCROLL:
    {
        size_t i = 0;
        for (; i < room->consumables.scroll_num && !check_equal_coords(consumable_coords->coordinates, room->consumables.scrolls[i].geometry.coordinates); i++);
        if (i != room->consumables.scroll_num)
        {
            room->consumables.scrolls[i] = room->consumables.scrolls[room->consumables.scroll_num - 1];
            room->consumables.scroll_num--;
        }
        break;
    }

    case WEAPON:
    {
        size_t i = 0;
        for (; i < room->consumables.weapon_num && !check_equal_coords(consumable_coords->coordinates, room->consumables.weapons[i].geometry.coordinates); i++);
        if (i != CONSUMABLES_TYPE_MAX_NUM)
        {
            room->consumables.weapons[i] = room->consumables.weapons[room->consumables.weapon_num - 1];
            room->consumables.weapon_num--;
        }
        break;
    }

    default: break;
    }
    room->consumables_num--;
}

void throw_current_weapon(player_t *player, room_t *current_room, weapon_t old_weapon)
{
    size_t i = 0;
    for (; i < player->backpack.weapon_num && !equal_weapons(&old_weapon, player->backpack.weapons + i); i++);

    weapon_t current_weapon = player->backpack.weapons[player->backpack.weapon_num - 1];
    player->backpack.weapons[player->backpack.weapon_num - 1] = player->backpack.weapons[i];
    player->backpack.weapons[i] = current_weapon;
    player->backpack.weapon_num--;
    throw_on_ground(player, current_room, old_weapon);
    player->backpack.current_size--;
}

void throw_on_ground(const player_t *player, room_t *current_room, weapon_t weapon)
{
    current_room->consumables.weapons[current_room->consumables.weapon_num].geometry = player->base_stats.coords;
    current_room->consumables.weapons[current_room->consumables.weapon_num].weapon = weapon;
    weapon_room_t room_weapon = current_room->consumables.weapons[current_room->consumables.weapon_num];
    move_character_by_direction(RIGHT, &room_weapon.geometry);
    for (directions_e direction = 0; character_outside_border(&room_weapon.geometry, &current_room->coords) || !check_unoccupied_room(&room_weapon.geometry, current_room); direction++)
    {
        room_weapon = current_room->consumables.weapons[current_room->consumables.weapon_num];
        move_character_by_direction(direction, &room_weapon.geometry);
    }
    current_room->consumables.weapons[current_room->consumables.weapon_num].geometry = room_weapon.geometry;
    current_room->consumables_num++;
    current_room->consumables.weapon_num++;
}

bool check_unoccupied_room(const object_t *coordinates, const room_t *room)
{
    bool unoccupied = true;
    for (size_t i = 0; i < room->consumables.elixir_num && unoccupied; i++)
        if (check_equal_coords(coordinates->coordinates, room->consumables.elixirs[i].geometry.coordinates))
            unoccupied = false;
    for (size_t i = 0; i < room->consumables.food_num && unoccupied; i++)
        if (check_equal_coords(coordinates->coordinates, room->consumables.room_food[i].geometry.coordinates))
            unoccupied = false;
    for (size_t i = 0; i < room->consumables.scroll_num && unoccupied; i++)
        if (check_equal_coords(coordinates->coordinates, room->consumables.scrolls[i].geometry.coordinates))
            unoccupied = false;
    for (size_t i = 0; i < room->consumables.weapon_num && unoccupied; i++)
        if (check_equal_coords(coordinates->coordinates, room->consumables.weapons[i].geometry.coordinates))
            unoccupied = false;
    for (size_t i = 0; i < room->monster_num && unoccupied; i++)
        if (check_equal_coords(coordinates->coordinates, room->monsters[i].base_stats.coords.coordinates))
            unoccupied = false;
    return unoccupied;
}

bool check_unoccupied_level(const object_t *coordinates, const level_t *level)
{
    bool unoccupied = true;
    for (size_t i = 0; unoccupied && i < ROOMS_NUM; i++)
        unoccupied = check_unoccupied_room(coordinates, level->rooms + i);
    if (check_equal_coords(coordinates->coordinates, level->end_of_level.coordinates))
        unoccupied = false;
    return unoccupied;
}

void use_consumable(player_t *player, consumable_types_e type, int consumable_pos, room_t *room)
{
    weapon_t old_weapon;
    switch (type)
    {
    case SCROLL: read_scroll(player, &player->backpack.scrolls[consumable_pos]);
                 remove_from_backpack(&player->backpack, consumable_pos, SCROLL);
                 player->backpack.current_size--;
                 break;
    case ELIXIR: drink_elixir(player, &player->backpack.elixirs[consumable_pos]);
                 remove_from_backpack(&player->backpack, consumable_pos, ELIXIR);
                 player->backpack.current_size--;
                 break;
    case FOOD:   eat_food(player, &player->backpack.foods[consumable_pos]);
                 remove_from_backpack(&player->backpack, consumable_pos, FOOD);
                 player->backpack.current_size--;
                 break;
    case WEAPON:
        if (consumable_pos == -1)
            player->weapon.strength = NO_WEAPON;
        else if (room && !equal_weapons(player->backpack.weapons + consumable_pos, &player->weapon) && player->weapon.strength != NO_WEAPON)
        {
            old_weapon = player->weapon;
            player->weapon = player->backpack.weapons[consumable_pos];
            throw_current_weapon(player, room, old_weapon);
        }
        else if (player->weapon.strength == NO_WEAPON)
            player->weapon = player->backpack.weapons[consumable_pos];
        else
            player->weapon = player->backpack.weapons[0];
        break;
    default:     break;
    }
}

void eat_food(player_t *player, const food_t *food)
{
    player->base_stats.health = player->base_stats.health + food->to_regen > player->regen_limit ? player->regen_limit : player->base_stats.health + food->to_regen;
}

void drink_elixir(player_t *player, const elixir_t *elixir)
{
    switch (elixir->stat)
    {
        case HEALTH:   player->elixir_buffs.max_health[player->elixir_buffs.current_health_buff_num].stat_increase += elixir->increase;
                       player->elixir_buffs.max_health[player->elixir_buffs.current_health_buff_num].effect_end = time(NULL) + elixir->duration;
                       player->regen_limit += elixir->increase;
                       player->base_stats.health += elixir->increase;
                       player->elixir_buffs.current_health_buff_num++;
                       break;
        case AGILITY:  player->elixir_buffs.agility[player->elixir_buffs.current_agility_buff_num].stat_increase += elixir->increase;
                       player->elixir_buffs.agility[player->elixir_buffs.current_agility_buff_num].effect_end = time(NULL) + elixir->duration;
                       player->base_stats.agility += elixir->increase;
                       player->elixir_buffs.current_agility_buff_num++;
                       break;
        case STRENGTH: player->elixir_buffs.strength[player->elixir_buffs.current_strength_buff_num].stat_increase += elixir->increase;
                       player->elixir_buffs.strength[player->elixir_buffs.current_strength_buff_num].effect_end = time(NULL) + elixir->duration;
                       player->base_stats.strength += elixir->increase;
                       player->elixir_buffs.current_strength_buff_num++;
                       break;
        default:
                       break;
    }
}

void read_scroll(player_t *player, const scroll_t *scroll)
{
    switch (scroll->stat)
    {
        case HEALTH:   player->regen_limit += scroll->increase;
                       player->base_stats.health += scroll->increase;
                       break;
        case AGILITY:  player->base_stats.agility += scroll->increase;
                       break;
        case STRENGTH: player->base_stats.strength += scroll->increase;
                       break;
        default:
                       break;
    }
}

void remove_from_backpack(backpack_t *players_backpack, size_t pos, consumable_types_e type)
{
    switch (type)
    {
        case SCROLL: players_backpack->scrolls[pos] = players_backpack->scrolls[players_backpack->scroll_num - 1];
                     players_backpack->scroll_num--;
                     break;

        case ELIXIR: players_backpack->elixirs[pos] = players_backpack->elixirs[players_backpack->elixir_num - 1];
                     players_backpack->elixir_num--;
                     break;

        case FOOD:   players_backpack->foods[pos] = players_backpack->foods[players_backpack->food_num - 1];
                     players_backpack->food_num--;
                     break;
        
        default:     break;
    }
}

void check_temp_effect_end(player_t *player)
{
    for (size_t i = 0; i < player->elixir_buffs.current_health_buff_num;)
        if (player->elixir_buffs.max_health[i].effect_end > time(NULL))
            i++;
        else
        {
            player->regen_limit -= player->elixir_buffs.max_health[i].stat_increase;
            player->base_stats.health -= player->elixir_buffs.max_health[i].stat_increase;
            if (player->base_stats.health <= 0)
                player->base_stats.health = 1;
            player->elixir_buffs.max_health[i] = player->elixir_buffs.max_health[player->elixir_buffs.current_health_buff_num - 1];
            player->elixir_buffs.current_health_buff_num--;
        }
    
    for (size_t i = 0; i < player->elixir_buffs.current_agility_buff_num;)
        if (player->elixir_buffs.agility[i].effect_end > time(NULL))
            i++;
        else
        {
            player->base_stats.agility -= player->elixir_buffs.agility[i].stat_increase;
            player->elixir_buffs.agility[i] = player->elixir_buffs.agility[player->elixir_buffs.current_agility_buff_num - 1];
            player->elixir_buffs.current_agility_buff_num--;
        }

    for (size_t i = 0; i < player->elixir_buffs.current_strength_buff_num;)
        if (player->elixir_buffs.strength[i].effect_end > time(NULL))
            i++;
        else
        {
            player->base_stats.strength -= player->elixir_buffs.strength[i].stat_increase;
            player->elixir_buffs.strength[i] = player->elixir_buffs.strength[player->elixir_buffs.current_strength_buff_num - 1];
            player->elixir_buffs.current_strength_buff_num--;
        }
}
