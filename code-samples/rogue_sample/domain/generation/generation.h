/**
 * @file
 * @brief Заголовочный файл для работы с процедурной генерацией
 * 
 * Данный файл содержит в себе функции для генерации очередного уровня:
 * - Генерация комнат
 * - Генерация коридоров
 * @author Бутаку Роман
 * @version 1.0
 * @date Последнее изменение: 15 июля 2024 года
*/

#ifndef GENERATION_H__
#define GENERATION_H__

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tools.h"
#include "random.h"
#include "dsu.h"
#include "graph.h"
#include "entities.h"
#include "entities_consts.h"
#include "consumables_actions.h"

typedef void (*generate_consumable_func)(room_t *room, const player_t *player);

/**
 * @brief Функция для генерации всех объектов уровня
 *
 * Данная функция выполняет генерацию комнат и коридоров
 * @param[in, out] level Структура с информацией о текущем уровне
 * @param[in, out] player Структура с информацией об игроке
*/
void generate_next_level(level_t *level, player_t *player);

/**
 * @brief Функция для генерации комнат уровня
 *
 * Данная функция выполняет генерацию комнат подземелья, все комнаты
 * имеют свои участки в пределах которых они могут генерироваться,
 * подземелье условно разделено на 9 участков.
 * @param[in, out] rooms Массив комнат
*/
void generate_rooms(room_t *rooms);

/**
 * @brief Функция для генерации всех возможных ребер между комнатами
 *
 * Данная функция получает на вход массив комнат и генерирует массив ребер,
 * которые могут связывать данные комнаты
 * @param[in, out] edges Массив ребер между комнатами
 * @param[in, out] count_edges Количество рёбер
*/
void generate_edges_for_rooms(edge_t *edges, size_t *count_edges);

/**
 * @brief Функция для добавления нового коридора исходя из его координат и размеров
 *
 * Данная функция добавляет новый коридор в массив коридоров
 * @param[in] coord_x Координата X левого верхнего угла
 * @param[in] coord_y Координата Y левого верхнего угла
 * @param[in] width Ширина нового коридора
 * @param[in] height Высота нового коридора
 * @param[in, out] passages Структура с информацией обо всех коридорах уровня
*/
void create_passage(int coord_x, int coord_y, int width, int height, passages_t *passages);

/**
 * @brief Функция для генерации горизонтальных коридоров между комнатами
 *
 * Данная функция получает на вход номера двух комнат и формирует между ними коридор
 * который может быть как прямым, так и с изгибами
 * @param[in] first_room Номер первой комнаты
 * @param[in] second_room Номер второй комнаты
 * @param[in] * @param[in, out] passages_array Структура с информацией обо всех коридорах уровня
 * @param[in, out] passages Структура с информацией обо всех коридорах уровня
*/
void generate_horizontal_passage(int first_room, int second_room, const room_t *room, passages_t *passages);

/**
 * @brief Функция для генерации вертикальных коридоров между комнатами
 *
 * Данная функция получает на вход номера двух комнат и формирует между ними коридор
 * который может быть как прямым, так и с изгибами
 * @param[in] first_room Номер первой комнаты
 * @param[in] second_room Номер второй комнаты
 * @param[in] rooms Массив комнат
 * @param[in, out] passage Структура с информацией обо всех коридорах уровня
*/
void generate_vertical_passage(int first_room, int second_room, const room_t *room, passages_t *passages);

/**
 * @brief Функция для генерации коридоров между комнатами
 *
 * Данная функция генерирует все возможные ребра между комнатами, затем на этих
 * ребрах строится дерево, на основании информации об этих ребрах строятся коридоры
 * между комнатами
 * @param[in] rooms Массив комнат
 * @param[in, out] passages Структура с информацией обо всех коридорах уровня
*/
void generate_passages(passages_t *passages, const room_t *rooms);

/**
 * @brief Функция для генерации координат сущности
 *
 * Данная функция генерирует координаты сущности для определенной комнаты
 * @param[in] room Комната, в которой будут генерироваться координаты
 * @param[in, out] coords Координаты сущности
*/
void generate_coords_of_entity(const room_t *room, object_t *coords);

/**
 * @brief Функция для генерации данных об игроке
 *
 * Данная функция генерирует координаты игрока на уровне и определяет
 * в какой комнате он будет находиться
 * @param[in] rooms Массив комнат
 * @param[in, out] player Структура с информацией об игроке
 * @return Номер в комнаты, в которой находится игрок
*/
int generate_player(const room_t *rooms, player_t *player);

/**
 * @brief Функция для генерации данных о выходе из очередного уровня
 *
 * Данная функция генерирует координаты выхода с условием, что он не будет находиться
 * в той же комнате, что и игрок
 * @param[in, out] level Структура с информацией об уровне
 * @param[in] player_room Номер комнаты, в которой находится игрок
*/
void generate_exit(level_t *level, int player_room);

/**
 * @brief Функция для генерации данных моснтра и установки начальных значений
 *
 * Данная функция определяет случайным образом тип монстра,
 * на основе этого определяет его враждебность
 * @param[in, out] monster Структура с информацией о монстре
 * @param[in] level_num Текущий уровень
*/
void generate_monster_data(monster_t *monster, int level_num);

/**
 * @brief Функция для генерации данных о монстрах на уровне
 *
 * Данная функция генерирует координаты и тип монстров с условием, что они не будут находиться
 * в той же комнате, что и игрок
 * @param[in, out] level Структура с информацией об уровне
 * @param[in] player_room Номер комнаты, в которой находится игрок
*/
void generate_monsters(level_t *level, int player_room);

/**
 * @brief Функция для генерации данных о еде
 *
 * Данная функция генерирует количество регенируемого здоровья при использовании еды
 * Регенерация зависит от здоровья игрока
 * @param[in, out] food Структура с информацией о еде
 * @param[in] player Структура с информацией об игроке
*/
void generate_food_data(food_t *food, const player_t *player);

/**
 * @brief Функция для генерации данных о еде на уровне
 *
 * Данная функция генерирует координаты еды и уровень ее регенерации
 * (в зависимости от максимального здоровья игрока)
 * для игрока с условием, что они не будут находиться в той же комнате, что и игрок
 * @param[in, out] room Структура с информацией о комнате
 * @param[in] player Струкутра с информацией об игроке
*/
void generate_food(room_t *room, const player_t *player);

/**
 * @brief Функция для генерации данных о эликсире
 *
 * Данная функция генерирует тип улучшаемой характеристики, а также увеличиваемое значение,
 * которое зависит от характеристик игрока
 * Также определяется время действия эликсира
 * @param[in, out] elixir Структура с информацией о эликсире
 * @param[in] player Структура с информацией об игроке
*/
void generate_elixir_data(elixir_t *elixir, const player_t *player);

/**
 * @brief Функция для генерации данных об эликсирах на уровне
 *
 * Данная функция генерирует координаты эликсиры, тип характеристики, которую они увеличивают,
 * а также время действия каждого эликсира
 * @param[in, out] room Структура с информацией о комнате
 * @param[in] player Струкутра с информацией об игроке
*/
void generate_elixir_data(elixir_t *elixir, const player_t *player);

/**
 * @brief Функция для генерации данных о эликсире
 *
 * Данная функция генерирует тип улучшаемой характеристики, а также увеличиваемое значение,
 * которое зависит от характеристик игрока
 * @param[in, out] elixir Структура с информацией о эликсире
 * @param[in] player Структура с информацией об игроке
*/
void generate_scroll_data(scroll_t *scroll, const player_t *player);

/**
 * @brief Функция для генерации данных о свитках на уровне
 *
 * Данная функция генерирует координаты свитки и тип характеристики, которую они увеличивают
 * @param[in, out] room Структура с информацией о комнате
 * @param[in] player Струкутра с информацией об игроке
*/
void generate_scroll_data(scroll_t *scroll, const player_t *player);

/**
 * @brief Функция для генерации данных об оружии
 *
 * Данная функция генерирует силу оружия
 * @param[in, out] weapon Структура с информацией об оружии
 * @param[in] player Структура с информацией об игроке
*/
void generate_weapon_data(weapon_t *weapon, const player_t *player);

/**
 * @brief Функция для генерации данных об оружии на уровне
 *
 * Данная функция генерирует координаты оружия и размер его урона
 * @param[in, out] room Структура с информацией о комнате
 * @param[in] player Струкутра с информацией об игроке
*/
void generate_weapon(room_t *room, const player_t *player);

/**
 * @brief Функция для генерации всех расходников на уровне
 *
 * Данная функция вызывает поочередно функции генерации еды, свитков, эликсиров и оружия
 * @param[in, out] level Структура с информацией об уровне
 * @param[in] player_room Номер комнаты, в которой находится игрок
 * @param[in] player Структура с информацией об игроке
 * @param[in] level_num Текущий уровень
*/
void generate_consumables(level_t *level, int player_room, const player_t *player, int level_num);

#endif // GENERATION_H__
