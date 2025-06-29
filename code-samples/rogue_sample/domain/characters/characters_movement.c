#include "characters_movement.h"

bool character_outside_border(const object_t *character_coords, const object_t *room)
{
    return (character_coords->coordinates[X] + (signed long) character_coords->size[X] - 1 >=\
     room->coordinates[X] + (signed long) room->size[X] - 1) || \
     (character_coords->coordinates[X] <= room->coordinates[X]) || \
     (character_coords->coordinates[Y] <= room->coordinates[Y]) || \
     (character_coords->coordinates[Y] + (signed long) character_coords->size[Y] - 1 >= \
     room->coordinates[Y] + (signed long) room->size[Y] - 1);
}

void move_character_by_direction(directions_e direction, object_t *character_geometry)
{
    switch (direction)
    {
    case FORWARD:                  character_geometry->coordinates[Y]--;
                                break;
    case LEFT:                     character_geometry->coordinates[X]--;
                                break;
    case RIGHT:                    character_geometry->coordinates[X]++;
                                break;
    case BACK:                     character_geometry->coordinates[Y]++;
                                break;
    case DIAGONNALY_FORWARD_LEFT:  character_geometry->coordinates[X]--;
                                    character_geometry->coordinates[Y]--;
                                break;
    case DIAGONALLY_FORWARD_RIGHT: character_geometry->coordinates[X]++;
                                    character_geometry->coordinates[Y]--;
                                break;
    case DIAGONALLY_BACK_LEFT:     character_geometry->coordinates[X]--;
                                    character_geometry->coordinates[Y]++;
                                break;
    case DIAGONALLY_BACK_RIGHT:    character_geometry->coordinates[X]++;
                                    character_geometry->coordinates[Y]++;
                                break;
    case STOP:                  break;
    }
}

void move_character_by_path(const vector *path, object_t *character_geometry)
{
    for (size_t i = 0; path && i < path->size; i++)
        move_character_by_direction(path->data[i], character_geometry);
}

void move_monster(monster_t *monster, const object_t *player_coordinates, const level_t *level)
{
    npc_movement_func npc_movement_functions[MONSTER_TYPE_NUM] = {
        [ZOMBIE] = pattern_zombie,
        [VAMPIRE] = pattern_vampire,
        [GHOST] = pattern_ghost,
        [OGRE] = pattern_ogre,
        [SNAKE] = pattern_snake
    };

    vector *path = NULL;
    if (is_player_near(player_coordinates, monster))
    {
        path = dist_and_next_pos_to_target(&monster->base_stats.coords, player_coordinates, level);
        if (path)
            path->size = 1;
    }
    if (!path)
        path = npc_movement_functions[monster->type](monster, level);

    object_t coords = monster->base_stats.coords;
    if (path)
    {
        move_character_by_path(path, &coords);
        if (!check_equal_coords(coords.coordinates, player_coordinates->coordinates))
            move_character_by_path(path, &monster->base_stats.coords);
        monster->dir = path->data[path->size - 1];
    }
    destroy_vector(&path);
}

vector *pattern_zombie(const monster_t *monster, const level_t *level)
{
    vector *path = create_vector();
    for (size_t try = 0; try < MAX_TRIES_TO_MOVE && is_empty_vector(path); try++)
    {
        object_t current_coords = monster->base_stats.coords;
        directions_e current_direction = rand() % SIMPLE_DIRECTIONS;
        move_character_by_direction(current_direction, &current_coords);
        if (!check_outside_border(&current_coords, level) && check_unoccupied_level(&current_coords, level))
            push_back(path, current_direction);
    }
    return path;
}

vector *pattern_vampire(const monster_t *monster, const level_t *level)
{
    vector *path = create_vector();
    for (size_t try = 0; try < MAX_TRIES_TO_MOVE && is_empty_vector(path); try++)
    {
        object_t current_coords = monster->base_stats.coords;
        directions_e current_direction = rand() % ALL_DIRECTIONS;
        move_character_by_direction(current_direction, &current_coords);
        if (!check_outside_border(&current_coords, level) && check_unoccupied_level(&current_coords, level))
            push_back(path, current_direction);
    }
    return path;
}

vector *pattern_ghost(const monster_t *monster, const level_t *level)
{
    vector *path = NULL;
    const room_t *room = NULL;
    for (size_t i = 0; i < ROOMS_NUM && !room; i++)
        if (!character_outside_border(&monster->base_stats.coords, &level->rooms[i].coords))
            room = &level->rooms[i];

    for (size_t try = 0; try < MAX_TRIES_TO_MOVE && !path && room; try++)
    {
        object_t current_coords = { .size[X] = 1, .size[Y] = 1, };
        current_coords.coordinates[X] = room->coords.coordinates[X] + get_random_in_range(1, room->coords.size[X] - 2);
        current_coords.coordinates[Y] = room->coords.coordinates[Y] + get_random_in_range(1, room->coords.size[Y] - 2);
        if (!check_outside_border(&current_coords, level) && check_unoccupied_level(&current_coords, level))
            path = dist_and_next_pos_to_target(&monster->base_stats.coords, &current_coords, level);
    }
    return path;
}

vector *pattern_ogre(const monster_t *monster, const level_t *level)
{
    vector *path = create_vector();
    for (size_t try = 0; try < MAX_TRIES_TO_MOVE && is_empty_vector(path); try++)
    {
        object_t current_coords = monster->base_stats.coords;
        directions_e current_direction = rand() % SIMPLE_DIRECTIONS;

        bool move = true;
        for (size_t i = 0; i < OGRE_STEP; i++)
        {
            move_character_by_direction(current_direction, &current_coords);
            if (check_outside_border(&current_coords, level) || !check_unoccupied_level(&current_coords, level))
                move = false;
        }
        for (size_t i = 0; i < OGRE_STEP && move; i++)
            push_back(path, current_direction);
    }
    return path;
}

vector *pattern_snake(const monster_t *monster, const level_t *level)
{
    vector *path = create_vector();
    for (size_t try = 0; try < MAX_TRIES_TO_MOVE && is_empty_vector(path); try++)
    {
        object_t current_coords = monster->base_stats.coords;
        directions_e current_direction = SIMPLE_TO_DIAGONAL_SHIFT + rand() % DIAGONAL_DIRECTIONS;
        move_character_by_direction(current_direction, &current_coords);
        if (current_direction == monster->dir)
            continue;
        if (!check_outside_border(&current_coords, level) && check_unoccupied_level(&current_coords, level))
            push_back(path, current_direction);
    }

    if (is_empty_vector(path))
    {
        object_t current_coords = monster->base_stats.coords;
        move_character_by_direction(monster->dir, &current_coords);
        if (!check_outside_border(&current_coords, level) && check_unoccupied_level(&current_coords, level))
            push_back(path, monster->dir);
    }

    return path;
}

bool is_player_near(const object_t *player_coordinates, const monster_t *monster)
{
    int dist = abs(player_coordinates->coordinates[X] - monster->base_stats.coords.coordinates[X]);
    dist += abs(player_coordinates->coordinates[Y] - monster->base_stats.coords.coordinates[Y]);

    bool player_near = false;
    switch (monster->hostility)
    {
    case LOW:     if (dist <= LOW_HOSTILITY_RADIUS)
                      player_near = true;
                  break;

    case AVERAGE: if (dist <= AVERAGE_HOSTILITY_RADIUS)
                      player_near = true;
                  break;
    
    case HIGH:    if (dist <= HIGH_HOSTILITY_RADIUS)
                      player_near = true;
                  break;
    }
    return player_near;
}

void move_player(player_t *player, const level_t *level, directions_e chosen_direction)
{
    coordinates_t current_coords = { player->base_stats.coords.coordinates[X], player->base_stats.coords.coordinates[Y] };
    switch (chosen_direction)
    {
        case FORWARD: player->base_stats.coords.coordinates[Y]--;
                        break;
        case BACK:    player->base_stats.coords.coordinates[Y]++;
                        break;
        case RIGHT:   player->base_stats.coords.coordinates[X]++;
                        break;
        case LEFT:    player->base_stats.coords.coordinates[X]--;
                        break;
        default:      break;
    }

    if (check_outside_border(&player->base_stats.coords, level))
    {
        player->base_stats.coords.coordinates[X] = current_coords[X];
        player->base_stats.coords.coordinates[Y] = current_coords[Y];
    }
}

bool check_outside_border(const object_t *player_coordinates, const level_t *level)
{
    bool outside_border = true;
    for (size_t i = 0; i < ROOMS_NUM && outside_border; i++)
        outside_border = character_outside_border(player_coordinates, &level->rooms[i].coords);
    for (size_t i = 0; i < level->passages.passages_num && outside_border; i++)
        outside_border = character_outside_border(player_coordinates, &level->passages.passages[i]);
    return outside_border;
}

room_t *find_current_room(const object_t *player_coords, level_t *level)
{
    for (size_t i = 0; i < ROOMS_NUM; i++)
        if (!character_outside_border(player_coords, &level->rooms[i].coords))
            return &level->rooms[i];
    return NULL;
}
