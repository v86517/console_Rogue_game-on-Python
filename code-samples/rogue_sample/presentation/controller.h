#ifndef CONTROLLER_H__
#define CONTROLLER_H__

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ncursesw/ncurses.h>
#include "presentation.h"
#include "data_actions.h"
#include "statistics.h"
#include "generation.h"
#include "screen.h"
#include "entities.h"
#include "entities_consts.h"
#include "characters_movement.h"
#include "consumables_actions.h"

#define ESCAPE ((char) 27)

void game_cycle(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *save, const char *score, const char *stat);

#endif // CONTROLLER_H__

