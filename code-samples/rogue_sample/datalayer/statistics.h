/**
 * @file
 * @brief Заголовочный файл для работы со статистикой по всем прохождениям
 * 
 * Для хранения используются файлы .json, их обработка происходит при помощи библиотеки json-c
 * @author Бутаку Роман
 * @version 1.0
 * @date Последняя дата изменения: 10 ноября 2024 года
*/

#ifndef STATISTICS_H__
#define STATISTICS_H__

#include <json.h>
#include <stdio.h>
#include <stddef.h>
#include "data_actions.h"

typedef struct
{
    size_t treasures;
    size_t level;
    size_t enemies;
    size_t food;
    size_t elixirs;
    size_t scrolls;
    size_t attacks;
    size_t missed;
    size_t moves;
} session_stat;

session_stat get_current_stat(const char *filename);

void save_session_stat(const session_stat *stat, const char *filename);

void update_statistics(const char *path_scoreboard, const char *path_stats);

session_stat *get_session_stat_array(const char *path_scoreboard, size_t *count_sessions);

int compare_session_stats(const void *first_ptr, const void *second_ptr);

#endif // STATISTICS_H__
