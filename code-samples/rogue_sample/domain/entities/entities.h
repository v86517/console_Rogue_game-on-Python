/**
 * @file
 * @brief Заголовочный файл с описанием сущностей игры
 * 
 * Данный файл содержит в себе описание пользовательских типов-сущностей, а также типы для координат в пространстве
 * @author Кантеров Роман
 * @version 0.2.3
 * @date Последнее изменение: 12 июля 2024 года
*/

#ifndef ENTITIES_H__
#define ENTITIES_H__

#include "entities_consts.h"
#include <stddef.h>
#include <time.h>
#include <stdbool.h>

/// Харктеристики виртуального пространства. Точка (0, 0) Находится в левом верхнем углу экран
typedef enum 
{
    X,
    Y,
    COORDS_NUM,
} dimension_e;

/// Возможные типы противников
typedef enum
{
    ZOMBIE,
    VAMPIRE, 
    GHOST,
    OGRE,
    SNAKE,
    MONSTER_TYPE_NUM,
} monster_type_e;

/// Уровни враждебности
typedef enum
{
    LOW, ///< Круг радиусом в LOW_HOSTILITY_RADIUS клетки
    AVERAGE, ///< Круг радиусом в AVERAGE_HOSTILITY_RADIUS клеток
    HIGH, ///< Круг радиусом в HIGH_HOSTILITY_RADIUS клеток
} hostility_type_e;

/// Типы характеристик
typedef enum
{
    HEALTH, 
    AGILITY, 
    STRENGTH,
    STAT_TYPE_NUM,
} stat_type_e;

/// Определяет, в каком направлении двигается объект
typedef enum
{
    FORWARD, ///< Вперед (^)
    BACK, ///< Назад (v)
    LEFT, ///< Влево (<)
    RIGHT, ///< Вправо (>)
    DIAGONNALY_FORWARD_LEFT, ///< Влево вверх по диагонали (<^)
    DIAGONALLY_FORWARD_RIGHT, ///< Вправо вверх по диагонали (>^)
    DIAGONALLY_BACK_LEFT, ///< Влево вниз по диагонали (<v)
    DIAGONALLY_BACK_RIGHT, ///< Вправо вниз по диагонали (>v)
    STOP, ///< Стоять на месте
} directions_e;

typedef int coordinates_t[COORDS_NUM];
typedef size_t sizes_t[COORDS_NUM];

/**
 * @struct
 * @brief Тип координат объекта игры, который используется для определения положения в пространстве конкретных объектов
*/
typedef struct
{
    coordinates_t coordinates; ///< Координаты объекта в пространстве, левая верхняя точка объекта
    sizes_t size; ///< Размеры объекта в формате X, Y
} object_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о характеристиках, имеющихся у каждого персонажа. Структуры монстр и игрок содержат в себе эту структуру
*/
typedef struct
{
    object_t coords; ///< Координаты персонажа
    double health; ///< Текущее количество здоровья
    signed agility; ///< Показатель ловкости
    signed strength; ///< Показатель силы
} character_t;

/**
 * @struct
 * @brief Тип, описывающий монстра
*/
typedef struct
{
    character_t base_stats; ///< Общие для всех персонажей характеристики
    monster_type_e type; ///< Тип монстра
    hostility_type_e hostility; ///< Уровень враждебности монстра
    bool is_chasing; ///< Флаг, устанавливающий, преследует ли монстр игрока
    directions_e dir; ///< Направление в котором двигался монстр (используется для змея, который должен постоянно менять направление движения)
} monster_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о сокровище
*/
typedef struct
{
    int value; ///< Ценность сокровища
} treasure_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о еде
*/
typedef struct
{
    int to_regen; ///< Количество восстанавливаемого здоровья
    char name[MAX_NAME_LEN]; ///< Название продукта
} food_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о зелье
*/
typedef struct
{
    time_t duration; ///< Длительность эффекта зелья
    stat_type_e stat; ///< Тип повышаемой характеристики
    int increase; ///< Количество единиц, на которые повышается выбранная характеристика
    char name[MAX_NAME_LEN]; ///< Название эликсира
} elixir_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о свитке
*/
typedef struct
{
    stat_type_e stat; ///< Тип повышаемой характеристики
    int increase; ///< Количество единиц, на которые повышается выбранная характеристика
    char name[MAX_NAME_LEN]; ///< Название свитка
} scroll_t;

/**
 * @struct
 * @brief Тип, содержащий информацию об оружии
*/
typedef struct
{
    int strength; ///< Количество единиц силы, на которые оружие повышает соответствующую характеристику владельца
    char name[MAX_NAME_LEN]; ///< Название оружия
} weapon_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о еде, находящейся в комнате
*/
typedef struct
{
    food_t food; ///< Характеристики еды
    object_t geometry; ///< Положение в пространстве
} food_room_t;

/**
 * @struct
 * @brief Тип, описывающий эликсир, лежащий в комнате
*/
typedef struct
{
    elixir_t elixir; ///< Характеристики эликсира
    object_t geometry; ///< Положение в пространстве
} elixir_room_t;

/**
 * @struct
 * @brief Тип, описывающий свиток, лежащий в комнате
*/
typedef struct
{
    scroll_t scroll; ///< Характеристики свитка
    object_t geometry; ///< Положение в пространстве
} scroll_room_t;

/**
 * @struct
 * @brief Тип, описывающий эликсир, лежащий в комнате
*/
typedef struct
{
    weapon_t weapon; ///< Характеристики оружия
    object_t geometry; ///< Положение в пространстве
} weapon_room_t;

/**
 * @struct
 * @brief Тип, описывающий все расходники в одной комнате
*/
typedef struct
{
    food_room_t room_food[MAX_CONSUMABLES_PER_ROOM]; ///< Массив еды
    size_t food_num; ///< Текущее количество еды

    elixir_room_t elixirs[MAX_CONSUMABLES_PER_ROOM]; ///< Массив эликсиров
    size_t elixir_num; ///< Текущее количество эликсиров

    scroll_room_t scrolls[MAX_CONSUMABLES_PER_ROOM]; ///< Массив свитков
    size_t scroll_num; ///< Текущее количество свитков

    weapon_room_t weapons[MAX_CONSUMABLES_PER_ROOM + CONSUMABLES_TYPE_MAX_NUM]; ///< Массив оружий
    size_t weapon_num; ///< Текущее количество оружия
} consumables_room_t;

/**
 * @struct
 * @brief Тип, содержащий информацию о рюкзаке игрока
*/
typedef struct
{
    size_t current_size; ///< Текущее количество заполненных слотов рюкзака

    food_t foods[CONSUMABLES_TYPE_MAX_NUM]; ///< Массив еды
    size_t food_num; ///< Количество еды

    elixir_t elixirs[CONSUMABLES_TYPE_MAX_NUM]; ///< Массив эликсиров
    size_t elixir_num; ///< Количество эликсиров

    scroll_t scrolls[CONSUMABLES_TYPE_MAX_NUM]; ///< Массив свитков
    size_t scroll_num; ///< Количество свитков

    treasure_t treasures; ///< Количество сокровищ

    weapon_t weapons[CONSUMABLES_TYPE_MAX_NUM]; ///< Массив оружий
    size_t weapon_num; ///< Количество оружия
} backpack_t;

/**
 * @struct
 * @brief Тип, описывающий информацию о временном бафе
*/
typedef struct
{
    int stat_increase; ///< Количество единиц, на которые повышается характеристика
    time_t effect_end; ///< Время действия эффекта (В секундах)
} buff_t;

typedef struct
{
    buff_t max_health[CONSUMABLES_TYPE_MAX_NUM]; ///< Баффы максимального уровня здоровья
    size_t current_health_buff_num; ///< Количество баффов максимального уровня здоровья

    buff_t agility[CONSUMABLES_TYPE_MAX_NUM]; ///< Баффы ловкости
    size_t current_agility_buff_num; ///< Количество баффов ловкости

    buff_t strength[CONSUMABLES_TYPE_MAX_NUM]; ///< Баффы силы
    size_t current_strength_buff_num; ///< Количество баффов силы
} buffs_t;

/**
 * @struct
 * @brief Тип, описывающий игрока
*/
typedef struct
{
    character_t base_stats; ///< Общие для всех персонажей характеристики
    int regen_limit; ///< Максимальное количество здоровья
    backpack_t backpack; ///< Рюкзак игрока
    weapon_t weapon; ///< Текущее оружие в руках игрока (в его отсутствии значению присваисвается значение NO_WEAPON)
    buffs_t elixir_buffs; ///< Временные баффы, действующие на игрока
} player_t;

/**
 * @struct
 * @brief Тип, описывающий комнату
*/
typedef struct
{
    object_t coords; ///< Геометрия комнаты
    consumables_room_t consumables; ///< Информация о расходниках, расположенных в комнате
    size_t consumables_num; ///< Число расходников
    monster_t monsters[MAX_MONSTERS_PER_ROOM]; ///< Информация о монстрах, расположенных в комнате
    size_t monster_num; ///< Число монстров
} room_t;

typedef object_t passage_t;

/**
 * @struct
 * @brief Тип, описывающий коридоры уровня
*/
typedef struct
{
    passage_t passages[MAX_PASSAGES_NUM]; ///< Массив коридоров, включая части коридоров
    size_t passages_num; ///< Количество коридоров
} passages_t;

/**ч
 * @struct
 * @brief Тип, описывающий уровень
*/
typedef struct
{
    object_t coords; ///< Геометрия уровня(===расширению экрана терминала)
    room_t rooms[ROOMS_NUM]; ///< Массив, хранящий информацию о комнатах
    passages_t passages; ///< Структура, хранящий информацию о коридорах
    int level_num; ///< Порядковый номер уровня
    object_t end_of_level; ///< Координаты выхода из уровня
} level_t;

#endif // ENTITIES_H__

