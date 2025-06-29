/**
 * @file
 * @brief Файл, содержащий константы, необходимые для определения сущностей
*/

#ifndef ENTITIES_CONSTS_H__
#define ENTITIES_CONSTS_H__

#define ROOMS_IN_WIDTH                                       3
#define ROOMS_IN_HEIGHT                                      3
#define ROOMS_NUM           (ROOMS_IN_WIDTH * ROOMS_IN_HEIGHT)
#define MAX_PASSAGE_PARTS                                    3
#define MAX_PASSAGES_NUM ((ROOMS_NUM - 1) * MAX_PASSAGE_PARTS)

#define REGION_WIDTH                     27
#define REGION_HEIGHT                    10
#define MIN_ROOM_WIDTH                    6
#define MAX_ROOM_WIDTH   (REGION_WIDTH - 2)
#define MIN_ROOM_HEIGHT                   5
#define MAX_ROOM_HEIGHT (REGION_HEIGHT - 2)

#define CONSUMABLES_TYPES_NUM                                          4
#define CONSUMABLES_TYPE_MAX_NUM                                       9
#define CONSUMABLES_NUM CONSUMABLES_TYPES_NUM * CONSUMABLES_TYPE_MAX_NUM

#define NO_WEAPON 0

#define MAX_NAME_LEN (32 + 1)

#define MAX_PERCENT_FOOD_REGEN_FROM_HEALTH 20
#define MAX_PERCENT_AGILITY_INCREASE       10
#define MAX_PERCENT_STRENGTH_INCREASE      10

#define MIN_ELIXIR_DURATION_SECONDS 30
#define MAX_ELIXIR_DURATION_SECONDS 60

#define MIN_WEAPON_STRENGTH 30
#define MAX_WEAPON_STRENGTH 50

#define LOW_HOSTILITY_RADIUS     2
#define AVERAGE_HOSTILITY_RADIUS 4
#define HIGH_HOSTILITY_RADIUS    6

#define MAX_CONSUMABLES_PER_ROOM 3
#define MAX_MONSTERS_PER_ROOM    2

#define LEVEL_UPDATE_DIFFICULTY 10
#define PERCENTS_UPDATE_DIFFICULTY_MONSTERS 2

#define LEVEL_NUM 21

#endif // ENTITIES_CONSTS_H__
