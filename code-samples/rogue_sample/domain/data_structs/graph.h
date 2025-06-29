/**
 * @file
 * @brief Заголовочный файл для работы с графами
 * 
 * Данный файл содержит в себе функции и структуры для работы с графами
 * @author Бутаку Роман
 * @version 1.0
 * @date Последнее изменение: 10 июля 2024 года
*/

#ifndef GRAPH_H__
#define GRAPH_H__

#include <stddef.h>
#include <stdbool.h>

#include "queue.h"
#include "vector.h"
#include "entities.h"
#include "entities_consts.h"
#include "characters_movement.h"

#define MAX_PATH_LEN 20

/**
 * @struct
 * @brief Структура невзвешенного ребра графа
*/
typedef struct edge
{
    int u; ///< Первая вершина ребра
    int v; ///< Вторая вершина ребра
} edge_t;

vector *dist_and_next_pos_to_target(const object_t *start, const object_t *target, const level_t *level);

#endif // GRAPH_H__

