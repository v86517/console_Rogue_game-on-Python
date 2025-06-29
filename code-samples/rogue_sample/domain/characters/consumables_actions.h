/**
 * @file
 * @brief Заголовочный файл, описывающий интерфейс взаимодействия игрока и расходников
 * 
 * В данном файле описаны механики подбора, потребления расходников
 * @author Кантеров Роман
 * @version 0.2.1
 * @date Последнее изменение: 16 июля 2024 года
*/

#ifndef CONSUMABLES_ACTIONS_H__
#define CONSUMABLES_ACTIONS_H__

#include "entities.h"
#include "characters_movement.h"
#include "characters_fight.h"
#include <stdbool.h>
#include <string.h>

/// @brief Типы расходников
typedef enum
{
    NONE,
    FOOD,
    ELIXIR,
    WEAPON,
    SCROLL,
} consumable_types_e;

/**
 * @brief Функция, проверяющая наличие расходников на позиции игрока
 * 
 * Последовательно проверяется каждый массив, хранящий данные о расходниках. При совпадении координат используется соответствующая @ref consumables_takes функция
 * @param[in, out] player Данные об игроке
 * @param[in, out] room Данные о комнате, в которой находится игрок
*/
void check_consumable(player_t *player, room_t *room);

/**
 * @defgroup consumables_takes Функции подбора расходников
 * @brief Модуль описывает функции, добавляющие предметы в рюкзак и удаляющие их из комнаты
 * 
 * Данные функции срабатывают каждый раз, когда игрок наступает на предмет
 * Перед добавлением проверяется переполнение рюкзака. Удаление из комнаты происходит путем стирание информации о расходнике из соответствующего массива
*/
///@{
    /**
     * @brief Функция подбора свитка в рюкзак
     * @param[in, out] players_backpack Данные о содержимом рюкзака игрока
     * @param[in, out] current_room Данные о комнате, в которой находится свиток
     * @param[in] scroll Данные о подбираемом свитке
    */
    void take_scroll(backpack_t *players_backpack, room_t *current_room, const scroll_room_t *scroll);

    /**
     * @brief Функция подбора эликсира в рюкзак
     * @param[in, out] players_backpack Данные о содержимом рюкзака игрока
     * @param[in, out] current_room Данные о комнате, в которой находится эликсир
     * @param[in] elixir Данные о подбираемом эликсире
    */
    void take_elixir(backpack_t *players_backpack, room_t *current_room, const elixir_room_t *elixir);

    /**
     * @brief Функция подбора еды в рюкзак
     * @param[in, out] players_backpack Данные о содержимом рюкзака игрока
     * @param[in, out] current_room Данные о комнате, в которой находится еда
     * @param[in] food Данные о подбираемой еде
    */
    void take_food(backpack_t *players_backpack, room_t *current_room, const food_room_t *food);

    /**
     * @brief Функция подбора оружия в рюкзак
     * 
     * Также выкидывает текущее выбранное оружие
     * @param[in, out] player Данные об игроке
     * @param[in, out] current_room Данные о комнате, в которой находится оружие
     * @param[in] weapon Данные о подбираемом оружии
    */
    void take_weapon(player_t *player, room_t *current_room, const weapon_room_t *weapon);
///@}


/**
 * @brief Функция определения и использования расходника
 * 
 * На основе переданного типа и порядкового номера в рюкзаке выбирается расходник, после чего вызываются соответствующая @ref consume функция
 * @param[in, out] player Данные об игроке
 * @param[in] type Тип расходника
 * @param[in] consumable_pos Порядковый номер в рюкзаке
*/
void use_consumable(player_t *player, consumable_types_e type, int consumable_pos, room_t *room);

/**
 * @defgroup consume Функции потребления расходников
 * @brief Модуль описывает функции, повышающие те или иные характеристики персонажа и удаления потребленных расходников
 * 
 * Удаление из комнаты происходит путем стирание информации о расходнике из соответствующего массива
*/
///@{
    /**
     * @brief Функция потребления еды
     * 
     * Результатом данной функции является увелечение показателя уровня текущего здоровья максимум до значения максимального здоровья
     * @param[in, out] player Данные об игроке
     * @param[in] food Данные о потребляемой еде
    */
    void eat_food(player_t *player, const food_t *food);

    /**
     * @brief Функция прочтения свитка
     * 
     * Результатом данной функции является увелечение одного из показателей игрока
     * @param[in, out] player Данные об игроке
     * @param[in] scroll Данные о свитке
    */
    void read_scroll(player_t *player, const scroll_t *scroll);

    /**
     * @brief Функция потребления эликсира
     * 
     * Результатом данной функции является временное увелечение одного из показателей игрока
     * @param[in, out] player Данные об игроке
     * @param[in] scroll Данные о выпиваемом эликсире
    */
    void drink_elixir(player_t *player, const elixir_t *elixir);
///@}

/**
 * @brief Функция удаления данных о расходнике из комнаты
 * 
 * Удаление происходит путем прохода по одному из массивов расходников в зависимости от переданного типа
 * @param[in, out] room Данные о комнате, в которой находится расходник
 * @param[in] consumable_coords Положение расходника в комнате
 * @param[in] type Тип расходника
*/
void delete_from_room(room_t *room, const object_t *consumable_coords, consumable_types_e type);

/**
 * @brief Функция, удаляющая экипированное оружие из рюкзака и располагающая его в комнате
 * 
 * Сначала функция сдвигает экипированное оружие в конец массива, хранящего оружие, выкидывает на землю при помощи @ref throw_on_ground() , после чего удаляет оружие из массива
 * @param[in, out] player Данные об игроке
 * @param[in, out] current_room Текущая комната, в которой располагается игрок
*/
void throw_current_weapon(player_t *player, room_t *current_room, weapon_t old_weapon);

/**
 * @brief Функция, располагающая оружие на полу
 * 
 * Функция определяет ближайшую соседнюю клетку, на которую можно расположить оружие. Если таковой нет, оружие располагается под игроком.
 * Предполагается, что выкидываемое оружие находится в конце списка
 * @param[in, out] player Данные об игроке
 * @param[in, out] current_room Комната, в которой расположен игрок
*/
void throw_on_ground(const player_t *player, room_t *current_room, weapon_t weapon);

/**
 * @brief Функция, проверяющая, занята ли клетка в комнате
 * 
 * Проверка выполняется при помощи последовательной проверки все элементов комнаты
 * @param[in] coordinates Проверяемые координаты
 * @param[in] room Комната, которая содержит данную координату
 * @return true, если точка не занята, false в обратном случае
*/
bool check_unoccupied_room(const object_t *coordinates, const room_t *room);

/**
 * @brief Функция, проверяющая, занята ли клетка на уровне
 * 
 * Проверка выполняется при помощи последовательной проверки все элементов комнаты
 * @param[in] coordinates Проверяемые координаты
 * @param[in] level Информация об уровне
 * @return true, если точка не занята, false в обратном случае
*/
bool check_unoccupied_level(const object_t *coordinates, const level_t *level);

/**
 * @brief Функция удаления расходника из рюкзака при его использовании
 * @param[in, out] players_backpack Информация о содержимом рюкзака
 * @param[in] pos Позиция расходника в рюкзаке (1-9)
 * @param[in] type Тип расходника
*/
void remove_from_backpack(backpack_t *players_backpack, size_t pos, consumable_types_e type);

/**
 * @brief Функция, удаляющая положительные эффекты зелий по истечении их времени действия
 * @param[in, out] player Данные об игроке
*/
void check_temp_effect_end(player_t *player);

#endif // CONSUMABLES_ACTIONS_H__
