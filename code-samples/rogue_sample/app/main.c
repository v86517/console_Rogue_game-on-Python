#include <stdbool.h>
#include <ncurses.h>
#include "screen.h"
#include "controller.h"
#include "presentation.h"
#include "generation.h"
#include "data_actions.h"

#define SAVE_PATH "./datalayer/data/save.json"
#define STATISTICS_PATH "./datalayer/data/statistics.json"
#define SCOREBOARD_PATH "./datalayer/data/scoreboard.json"

int main(void)
{
    srand(time(NULL));
    
    init_presentation();
    
    map_t map;
    player_t player;
    level_t level;
    battle_info_t battles[MAXIMUM_FIGHTS];

    start_screen();
    
    bool running_menu = true;
    int current_option = 0;
    while (running_menu)
    {
        menu_screen(current_option);
        char key = getch();
        switch (key)
        {
            case '\n':
                switch (current_option)
                {
                    case 0:
                        init_level(&player, &level, &map, battles, STATISTICS_PATH);
                        game_cycle(&player, &level, &map, battles, SAVE_PATH, SCOREBOARD_PATH, STATISTICS_PATH);
                        break;
                    case 1:
                        load_data(&player, &level, &map, battles, SAVE_PATH, STATISTICS_PATH);
                        game_cycle(&player, &level, &map, battles, SAVE_PATH, SCOREBOARD_PATH, STATISTICS_PATH);
                        break;
                    case 2:
                        display_scoreboard(SCOREBOARD_PATH);
                        while (getch() != ESCAPE);
                        break;
                    case 3:
                        running_menu = false;
                        break;
                }
                break;
            case 'W':
            case 'w':
                current_option = max(0, current_option - 1);
                break;
            case 'S':
            case 's':
                current_option = min(3, current_option + 1);
                break;
        }
    }

    endwin();
    return 0;
}
