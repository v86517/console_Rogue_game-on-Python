#ifndef SCREEN_H__
#define SCREEN_H__

#include <stdio.h>
#include <string.h>
#include <ncurses.h>

void start_screen(void);
void menu_screen(int current_line);
void dead_screen(void);
void endgame_screen(void);

#endif // SCREEN_H__
