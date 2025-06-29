/**
 * @file
 * @brief Заголовочный файл, описывающий интерфейс боя
 * 
 * В данном файле описаны все механики, использующиеся во время боя
 * @author Кантеров Роман
 * @version 0.4
 * @date Последняя дата изменения: 16 июля 2024 года
*/


#ifndef CHARACTERS_FIGHT_H__
#define CHARACTERS_FIGHT_H__

#include <stdbool.h>
#include <stdlib.h>
#include "entities.h"
#include "characters_movement.h"
#include "characters_fight_consts.h"

typedef enum
{
    PLAYER,
    MONSTER,
} turn_e;

typedef struct
{
    bool is_fight; // = false
    monster_t *enemy;
    bool vampire_first_attack; // = true
    bool ogre_cooldown; // = false
    bool player_asleep; // = true
} battle_info_t;

typedef double (*damage_formulas_func)(battle_info_t *);

/**
 * @brief Функция, описывающая попытку атаки
 * 
 * Эта функция объединяет в себе всю структуру атаки: проверка на попадание, расчет урона, нанесение урона.
 * @param[in, out] player Данные об игроке
 * @param[in, out] battle_info Данные о бое
 * @param[in] curr_turn Определяет, чья очередь выполнить атаку
*/
void attack(player_t *player, battle_info_t *battle_info, turn_e curr_turn);

/**
 * @brief Функция, высчитыающая, произойдёт ли попадание
 * 
 * Расчет определяется по формуле TODO
 * @param[in, out] player Данные об игроке
 * @param[in, out] monster Данные о монстре
 * @param[in] curr_turn Определяет, чья очередь выполнить атаку
 * @return true, если аткующий попал по цели, иначе false
*/
bool check_hit(const player_t *player, const monster_t *monster, turn_e curr_turn);

/**
 * @brief Функция, высчитывающая урон
 * 
 * TODO
 * @param[in, out] player Данные об игроке
 * @param[in, out] battle_info Данные о бое
 * @param[in] curr_turn Определяет, чья очередь выполнить атаку
 * @return количество урона, наносимого противнику
*/
double calculate_damage(const player_t *player, battle_info_t *battle_info, turn_e curr_turn);

/**
 * @brief Функция, определяющая количество сокровищ, получаемых игроком за убийства противника
 * 
 * Количество зависит от сложности противника
 * @param[in] monster данные о монстре
 * @return стоимость сокровища
*/
unsigned calculate_loot(const monster_t *monster);

/**
 * @brief Функция, удаляющая информацию о монстре из игры
 * 
 * Удаление происходит путем стирания информации из массива с сохранением изначального порядка
 * @param[in, out] room Данные о комнате, к которой принадлежит монстр
 * @param[in] monster Данные о монстре, который должен быть удален
*/
void delete_monster_info(room_t *room, const monster_t *monster);

/**
 * @brief Функция обновления статуса боёв
 * 
 * Сначала функция проверяет монстров на контакт с игроком (check_contact()) и создает структуру боя при помощи init_battle(), если контакт произошел.
 * После функция проверяет уже идущие бои на их завершение(игрок убежал (функция check_contact()), монстр умер) и деинициализирует эти записи
 * @param[in] player_coordinates Текущее положение игрока в пространстве
 * @param[in] level Данные о начинке уровне
 * @param[in, out] battles_array Массив, содержащий инфу о всех боях
*/
void update_fight_status(const object_t *player_coordinates, level_t *level, battle_info_t *battles_array);

/**
 * @brief Функция, записывающая информацию о бое в структуру
 * 
 * Выбирается первая доступная структура (доступной считается структура с флажком is_fight = false)
 * @param[in] monster Информация о противнике игрока
 * @param[in, out] battles_array Массив, содержащий инфу о всех боях
*/
void init_battle(monster_t *monster, battle_info_t *battles_array);

/**
 * @brief Функция проверки контакта игрока с противником
 * 
 * Функция смотрит, чтобы игрок был на расстоянии одной клетки от противника
 * @param[in] player_coordinates Координаты игрока
 * @param[in] monster Данные о противнике
 * @return true, если контакт есть, false в ином случае
*/
bool check_contact(const object_t *player_coordinates, const monster_t *monster);

/**
 * @brief Функция проверки, атаковал ли игрок противника
 * 
 * Функция проверяет, походил ли игрок на моба, и если да, то вызывает функцию attack() для игрока
 * @param[in] player Данные об игроке
 * @param[in, out] battle Данные о битве
 * @param[in] player_chosen_direction Выбранное игроком направление хода
 * @return true, если игрок попытался совершить атаку, false в ином случае
*/
bool check_player_attack(player_t *player, battle_info_t *battle, directions_e player_chosen_direction);

/**
 * @brief Функция, очищающая данные о монстрах
 * 
 * Функция проходится по комнатам уровня, проверяя хп каждого монстра, если оно неположительно, то удаляет данные о нем
 * @param[in, out] level Информация об уровне
*/
void remove_dead_monsters(level_t *level);

/**
 * @brief Функция проверки на совпадение координат
 * @param[in] first_coords Координаты первого объекта
 * @param[in] second_coords Координаты второго объекта
 * @return true, если координаты совпали, false в ином случае
*/
bool check_equal_coords(const coordinates_t first_coords, const coordinates_t second_coords);

/**
 * @brief Функция проверки на соседство координат
 * @param[in] first_coords Координаты первого объекта
 * @param[in] second_coords Координаты второго объекта
 * @return true, если координаты примыкают друг к другу, false в ином случае
*/
bool check_if_neighbor_tile(const coordinates_t first_coords, const coordinates_t second_coords);

/**
 * @brief Функция проверки на диагональное соседство координат координат
 * @param[in] first_coords Координаты первого объекта
 * @param[in] second_coords Координаты второго объекта
 * @return true, если координаты соединены по диагонали, false в ином случае
*/
bool check_if_diagonally_neighbour_tile(const coordinates_t first_coords, const coordinates_t second_coords);

/**
 * @brief Функция проверки на существование боя
 * 
 * Функция проверяет на совпадения данные монстра, который потенциально может создать новую запись о бое с уже существующими
 * @param[in] monster Данные о монстре
 * @param[in] battles_array Данные о боях
*/
bool check_unique(const monster_t *monster, const battle_info_t *battles_array);

/**
 * @brief Функция вычисления шанса попадания
 * 
 * Завсисит от ловкости и скорости атакующего и цели
 * @param[in] attacker_agility ловкость атакующего
 * @param[in] target_agility ловкость цели
 * @return Шанс попадания
*/
double hit_chance_formula(int attacker_agility, int target_agility);

/**
 * @defgroup monster_attack Формулы атаки монстров
 * @brief Модуль описывает функции, описывающие формулы, по которым вычисляется урон монстров
 * 
 * Урон зависит от силы (за исключением вампира), также для некоторых из монстров присутствуют униальные элементы, такие как шанс усыпить игрока у змеи
*/
///@{
    /**
     * @brief Функция вычисления урона вампира
     * 
     * Вампир отнимает значение MAX_PART_HP от максимального здоровья игрока
     * @param[in] player Информация об игроке
     * @return Количество урона, наносимое монстром игроку
    */
    double vampire_damage_formula(const player_t *player);

    /**
     * @brief Функция вычисления урона зомби и призрака
     * @param[in] battle_info Информация о бое
     * @return Количество урона, наносимое монстром игроку
    */
    double zombie_ghost_damage_formula(battle_info_t *battle_info);

    /**
     * @brief Функция вычисления урона огра
     * 
     * Огр атакует раз в два хода, при этом гарантированно попадая в игрока
     * @param[in] battle_info Информация о бое
     * @return Количество урона, наносимое монстром игроку
    */
    double ogre_damage_formula(battle_info_t *battle_info);

    /**
     * @brief Функция вычисления урона змеи
     * 
     * Змея с шансом SLEEP_CHANCE процентов может усыпить игрока на один ход
     * @param[in] battle_info Информация о бое
     * @return Количество урона, наносимое монстром игроку
    */
    double snake_damage_formula(battle_info_t *battle_info);
///@}

/// @brief Функция получения координат монстра
/// @param monster Информация о монстре
/// @return координаты, в которых находится монстр
int *get_monster_coordinates(monster_t *monster);

#endif // CHARACTERS_FIGHT_H__
