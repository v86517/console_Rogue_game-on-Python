#ifndef PRESENTATION_CONSTS_H__
#define PRESENTATION_CONSTS_H__

#include <stdbool.h>
#include <ncurses.h>
#include "entities_consts.h"

#define MAP_HEIGHT (ROOMS_IN_HEIGHT * REGION_HEIGHT)
#define MAP_WIDTH  (ROOMS_IN_WIDTH * REGION_WIDTH)

#define MAX_SCOREBOARD_SIZE 10

typedef struct
{
    chtype map[MAP_HEIGHT][MAP_WIDTH];
    bool visible_rooms[ROOMS_NUM];
    bool visible_passages[MAX_PASSAGES_NUM];
} map_t;

typedef enum
{
    WHITE_FONT = 1,
    RED_FONT,
    GREEN_FONT,
    BLUE_FONT,
    YELLOW_FONT,
    CYAN_FONT
} font_e;

#endif // PRESENTATION_CONSTS_H__
