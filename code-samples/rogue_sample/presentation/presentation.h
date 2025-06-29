#ifndef PRESENTATION_H__
#define PRESENTATION_H__

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <ncurses.h>
#include "entities.h"
#include "statistics.h"
#include "entities_consts.h"
#include "characters_movement.h"
#include "presentation_consts.h"

void init_presentation(void);
void clear_map_data(map_t *map);
void create_new_map(map_t *map, const level_t *level, const player_t *player, const battle_info_t *battles);
void display_map(map_t *map, const level_t *level, const player_t *player, const battle_info_t *battles);
void display_scoreboard(const char *path_scoreboard);

#endif // PRESENTATION_H__
