#include "characters_fight.h"

void attack(player_t *player, battle_info_t *battle_info, turn_e curr_turn)
{
    switch (curr_turn)
    {
    case PLAYER:
        if (check_hit(player, battle_info->enemy, PLAYER))
            battle_info->enemy->base_stats.health -= calculate_damage(player, battle_info, PLAYER);
        if (battle_info->enemy->base_stats.health <= 0)
            player->backpack.treasures.value += calculate_loot(battle_info->enemy);
        break;
        
    case MONSTER:
        if (check_hit(player, battle_info->enemy, MONSTER))
            player->base_stats.health -= calculate_damage(player, battle_info, MONSTER);
        break;
    }
}

bool check_hit(const player_t *player, const monster_t *monster, turn_e curr_turn)
{
    bool was_hit = false;
    int chance = INITIAL_HIT_CHANCE;
    switch (curr_turn)
    {
    case PLAYER:
        chance += hit_chance_formula(player->base_stats.agility, monster->base_stats.agility);
        break;
    
    case MONSTER:
        chance += hit_chance_formula(monster->base_stats.agility, player->base_stats.agility);
        break;
    }
    if (rand() % 100 < chance || monster->type == OGRE)
        was_hit = true;
    return was_hit;
}

double calculate_damage(const player_t *player, battle_info_t *battle_info, turn_e curr_turn)
{
    double damage = INITIAL_DAMAGE;
    static damage_formulas_func monster_damage_formulas[MONSTER_TYPE_NUM] = {
        [ZOMBIE] = zombie_ghost_damage_formula,
        [VAMPIRE] = NULL,
        [GHOST] = zombie_ghost_damage_formula,
        [OGRE] = ogre_damage_formula,
        [SNAKE] = snake_damage_formula
    };
    switch (curr_turn)
    {
    case PLAYER:
        if (!(battle_info->enemy->type == VAMPIRE && battle_info->vampire_first_attack) && !(battle_info->enemy->type == SNAKE && battle_info->player_asleep))
            if (player->weapon.strength == NO_WEAPON)
                damage += (player->base_stats.strength - STANDART_STRENGTH) * STRENGTH_FACTOR; 
            else
                damage = player->weapon.strength * (player->base_stats.strength + STRENGTH_ADDITION) / 100;
        else if (battle_info->enemy->type == VAMPIRE && battle_info->vampire_first_attack)
            battle_info->vampire_first_attack = false;
        else
            battle_info->player_asleep = false;
        break;
    
    case MONSTER:
        if (battle_info->enemy->type == VAMPIRE)
            damage = vampire_damage_formula(player);
        else
            damage = monster_damage_formulas[battle_info->enemy->type](battle_info);
        break;
    }
    return damage;
}

unsigned calculate_loot(const monster_t *monster)
{
    unsigned loot = monster->base_stats.agility * LOOT_AGILITY_FACTOR + monster->base_stats.health * LOOT_HP_FACTOR + monster->base_stats.strength * LOOT_STRENGTH_FACTOR + rand() % 20;
    return loot;
}

void delete_monster_info(room_t *room, const monster_t *monster)
{
    size_t current_pos = 0;
    for (size_t i = 0; i < room->monster_num; i++)
    {
        room->monsters[current_pos] = room->monsters[i];
        if (!check_equal_coords(monster->base_stats.coords.coordinates, room->monsters[i].base_stats.coords.coordinates))
            current_pos++;
    }
    room->monster_num--;
}

void update_fight_status(const object_t *player_coordinates, level_t *level, battle_info_t *battles_array)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
    {
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
        {
            if (check_contact(player_coordinates, &level->rooms[i].monsters[j]) && check_unique(&level->rooms[i].monsters[j], battles_array))
                init_battle(&level->rooms[i].monsters[j], battles_array);
        }
    }

    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
    {
        if (battles_array[i].is_fight && (!check_contact(player_coordinates, battles_array[i].enemy) || battles_array[i].enemy->base_stats.health <= 0))
            battles_array[i].is_fight = false;
    }
}

void init_battle(monster_t *monster, battle_info_t *battles_array)
{
    for (size_t i = 0; i < MAXIMUM_FIGHTS; i++)
    {
        if (!battles_array[i].is_fight)
        {
            battles_array[i].is_fight = true;
            battles_array[i].ogre_cooldown = false;
            battles_array[i].vampire_first_attack = true;
            battles_array[i].player_asleep = false;
            battles_array[i].enemy = monster;
            break;
        }
    }
}

bool check_contact(const object_t *player_coordinates, const monster_t *monster)
{
    bool is_contact = (check_if_neighbor_tile(player_coordinates->coordinates, monster->base_stats.coords.coordinates));
    if (!is_contact)
        is_contact = (monster->type == SNAKE && check_if_diagonally_neighbour_tile(player_coordinates->coordinates, monster->base_stats.coords.coordinates));
    return is_contact;
}

bool check_player_attack(player_t *player, battle_info_t *battle, directions_e player_chosen_direction)
{
    bool player_is_attacking = false;
    object_t old_coords = player->base_stats.coords;
    move_character_by_direction(player_chosen_direction, &old_coords);
    if (check_equal_coords(old_coords.coordinates, battle->enemy->base_stats.coords.coordinates))
    {
        attack(player, battle, PLAYER);
        player_is_attacking = true;
    }
    return player_is_attacking;
}

void remove_dead_monsters(level_t *level)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
        for (size_t j = 0; j < level->rooms[i].monster_num; j++)
        {
            if (level->rooms[i].monsters[j].base_stats.health <= 0)
                delete_monster_info(level->rooms + i, level->rooms[i].monsters + j);
        }
}

bool check_equal_coords(const coordinates_t first_coords, const coordinates_t second_coords)
{
    return (first_coords[X] == second_coords[X] && first_coords[Y] == second_coords[Y]);
}

bool check_if_neighbor_tile(const coordinates_t first_coords, const coordinates_t second_coords)
{
    return (first_coords[X] == second_coords[X] && abs(first_coords[Y] - second_coords[Y]) == 1)
            || (first_coords[Y] == second_coords[Y] && abs(first_coords[X] - second_coords[X]) == 1);
}

bool check_if_diagonally_neighbour_tile(const coordinates_t first_coords, const coordinates_t second_coords)
{
    return (abs(first_coords[X] - second_coords[X]) == 1 && abs(first_coords[Y] - second_coords[Y]) == 1);
}

bool check_unique(const monster_t *monster, const battle_info_t *battles_array)
{
    bool is_unique = true;
    for (size_t i = 0; i < MAXIMUM_FIGHTS && is_unique; i++)
        if (battles_array[i].is_fight && check_equal_coords(battles_array[i].enemy->base_stats.coords.coordinates, monster->base_stats.coords.coordinates))
            is_unique = false;
    return is_unique;
}

double hit_chance_formula(int attacker_agility, int target_agility)
{
    return (attacker_agility - target_agility - STANDART_AGILITY) * AGILITY_FACTOR;
}

double vampire_damage_formula(const player_t *player)
{
    return player->regen_limit / MAX_HP_PART;
}

double zombie_ghost_damage_formula(battle_info_t *battle_info)
{
    return INITIAL_DAMAGE + (battle_info->enemy->base_stats.strength - STANDART_STRENGTH) * STRENGTH_FACTOR;
}

double ogre_damage_formula(battle_info_t *battle_info)
{
    double damage = 0;
    if (!battle_info->ogre_cooldown)
    {
        damage = (battle_info->enemy->base_stats.strength - STANDART_STRENGTH) * STRENGTH_FACTOR;
        battle_info->ogre_cooldown = true;
    }
    else
        battle_info->ogre_cooldown = false;
    return damage;
}

double snake_damage_formula(battle_info_t *battle_info)
{
    if (rand() % 100 < SLEEP_CHANCE)
        battle_info->player_asleep = true;
    return zombie_ghost_damage_formula(battle_info);
}

int *get_monster_coordinates(monster_t *monster)
{
    return (monster->base_stats.coords.coordinates);
}
