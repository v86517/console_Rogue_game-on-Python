/**
 * @file
 * @brief Заголовочный файл, описывающий хранение и извлечение данных
 * 
 * Для хранения используются файлы .json, их обработка происходит при помощи библиотеки json-c
 * @author Кантеров Роман, Бутаку Роман
 * @version 2.0
 * @date Последняя дата изменения: 3 ноября 2024 года
*/

#ifndef DATA_ACTIONS_H__
#define DATA_ACTIONS_H__

#include <json.h>
#include "entities.h"
#include "generation.h"
#include "presentation.h"
#include "presentation_consts.h"
#include "statistics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Фунция загрузки сохранения в игру
 * 
 * Функция загружает данные об игроке, полученные из сохранения
 * @param[out] player Структура, куда записываются загруженные данные об игроке
 * @param[in] save_path Имя файла, из которого происходит загрузка
 * @param[in] stat_path Имя файла, где сохраняется статистика по прохождениям
 * @return текущий номер уровня
*/
void load_data(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *save_path, const char *stat_path);

/**
 * @brief Фунция загрузки сохранения в файл
 * 
 * Функция загружает данные об игроке, полученные по результатам игры
 * @param[in] level_num Данные о нномере уровня
 * @param[in] player Данные об игроке
 * @param[in] filename Путь к файлу сохранения
*/
void save_data(const player_t *player, const level_t *level, const map_t *map, const char *filename);

char *read_file_to_str(const char *filename);

/**
 * @brief Фунция инициализации игровой сессии
 * 
 * Функция задает начальные параметры основным сущностям игровой сессии
 * @param[in, out] player Данные об игроке
 * @param[in, out] level Данные об уровне
 * @param[in, out] map Данные о карте уровня
*/
void init_level(player_t *player, level_t *level, map_t *map, battle_info_t *battles, const char *stat_path);

/**
 * @brief Фунция загрузки базовых данных в файл
 * 
 * Функция загружает данные об игроке, которые он имеет на момент начала игры
 * @param[in] filename Путь к файлу сохранения
*/
void get_standart_save(const char *save_path, const char *stat_path);

#endif // DATA_ACTIONS_H__
