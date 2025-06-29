/**
 * @file
 * @brief Заголовочный файл для работы со случайными данными
 * 
 * Данный файл содержит в себе все функции для получения случайного числа в диапазоне,
 * а также реализацию случайной перестановки массива
 * @author Бутаку Роман
 * @version 1.0
 * @date Последнее изменение: 10 июля 2024 года
*/

#ifndef RANDOM_H__
#define RANDOM_H__

#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Функция для получения случайного числа в заданном отрезке
 *
 * @param[in] min Нижняя граница отрезка
 * @param[in] max Верхняя граница отрезка
 * @return Случайное число в диапазоне [min; max]
*/
int get_random_in_range(int min, int max);

/**
 * @brief Функция для обмена значениями двух переменных любых типов
 *
 * Данная функция принимает два адреса, а также размер в байтах
 * типа данных, значения которых надо обменять
 * @param[in] first Указатель на первую переменную
 * @param[in] second Указатель на вторую переменную
 * @param[in] size_of_elem Размер в байтах одной переменной
*/
void swap(void *first, void *second, size_t size_of_elem);

/**
 * @brief Функция для получения случайной перестановки массива
 * @param[in] array Указатель на массив
 * @param[in] size Количество элементов массива
 * @param[in] size_of_elem Размер в байтах одной переменной
*/
void shuffle_array(void *array, size_t size, size_t size_of_elem);

#endif // RANDOM_H__

