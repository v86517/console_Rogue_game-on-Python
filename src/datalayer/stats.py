import json  # Импортируем модуль json для работы с JSON-файлами
from pathlib import Path  # Импортируем класс Path из модуля pathlib для работы с путями

from utils.utils import get_project_root  # Импортируем функцию get_project_root из модуля utils.utils

class RogueStats:  # Определяем класс RogueStats для хранения статистики игры
    __instance = None  # Приватное статическое поле для хранения единственного экземпляра класса
    __save_dir = get_project_root() / "datasets/saves"  # Путь к директории для сохранений
    __stats_dir = get_project_root() / "datasets/stats"  # Путь к директории для статистики
    __stats_file = "stats.json"  # Имя файла для статистики
    __save_file = "saves.json"  # Имя файла для сохранений

    def __new__(cls, *args, **kwargs):  # Переопределяем метод __new__ для реализации паттерна Singleton
        if cls.__instance is None:  # Если экземпляр не создан
            cls.__instance = super().__new__(cls)  # Создаем новый экземпляр
        return cls.__instance  # Возвращаем единственный экземпляр

    def __init__(self):  # Конструктор класса RogueStats
        self.nickname = "Rogue"  # Инициализируем никнейм персонажа
        self.gold: int = 0  # Инициализируем количество золота
        self.rogue_level: int = 1  # Инициализируем уровень персонажа
        self.defeated_enemies: int = 0  # Инициализируем количество поверженных врагов
        self.eaten_food: int = 0  # Инициализируем количество съеденной еды
        self.used_potions: int = 0  # Инициализируем количество использованных зелий
        self.used_scrolls: int = 0  # Инициализируем количество использованных свитков
        self.total_hits: int = 0  # Инициализируем количество нанесенных ударов
        self.missed_hits: int = 0  # Инициализируем количество промахов
        self.passed_cells: int = 0  # Инициализируем количество пройденных клеток

    @classmethod
    def get_instance(cls):  # Метод для получения единственного экземпляра класса
        return cls.__instance  # Возвращаем единственный экземпляр

    def __get_stats_file(self) -> Path:  # Метод для получения пути к файлу статистики
        if not self.__stats_dir.exists():  # Если директория для статистики не существует
            raise FileExistsError(f"Stats directory does not exist: {self.__stats_dir}")  # Выбрасываем исключение

        for f in self.__stats_dir.iterdir():  # Проходим по всем файлам в директории
            if f.name == self.__stats_file:  # Если находим файл статистики
                return f  # Возвращаем путь к файлу

        f_path = self.__stats_dir / self.__stats_file  # Формируем путь к файлу статистики
        f_path.touch()  # Создаем файл, если он не существует

        return f_path  # Возвращаем путь к файлу

    def __get_save_file(self) -> Path:  # Метод для получения пути к файлу сохранений
        if not self.__save_dir.exists():  # Если директория для сохранений не существует
            raise FileExistsError(f"Save directory does not exist: {self.__save_dir}")  # Выбрасываем исключение

        for f in self.__save_dir.iterdir():  # Проходим по всем файлам в директории
            if f.name == self.__save_file:  # Если находим файл сохранений
                return f  # Возвращаем путь к файлу

        f_path = self.__save_dir / self.__save_file  # Формируем путь к файлу сохранений
        f_path.touch()  # Создаем файл, если он не существует

        return f_path  # Возвращаем путь к файлу

    def form_stats_dict(self) -> dict:  # Метод для формирования словаря со статистикой
        return {
            "last_record": True,  # Флаг последней записи
            "Никнейм": self.nickname,  # Никнейм персонажа
            "Золото": self.gold,  # Количество золота
            "Уровень подземелья": self.rogue_level,  # Уровень персонажа
            "Поверженные враги": self.defeated_enemies,  # Количество поверженных врагов
            "Съеденной еды": self.eaten_food,  # Количество съеденной еды
            "Использованных зелий": self.used_potions,  # Количество использованных зелий
            "Использованных свитков": self.used_scrolls,  # Количество использованных свитков
            "Нанесенных ударов": self.total_hits,  # Количество нанесенных ударов
            "Промахов": self.missed_hits,  # Количество промахов
            "Пройденных клеток": self.passed_cells,  # Количество пройденных клеток
        }

    def check_nickname(self, nickname: str) -> bool:  # Метод для проверки существования никнейма
        """
        Проверить существует ли игрок с таким ником.
        :return: True если свободен, False если занят.
        """
        stats_file = self.__get_stats_file()  # Получаем путь к файлу статистики
        try:
            with stats_file.open(mode="r", encoding="utf-8") as rd_f:  # Открываем файл для чтения
                data = json.load(rd_f)  # Загружаем данные из файла
                for dct in data:  # Проходим по всем записям
                    if dct["Никнейм"] == nickname:  # Если находим никнейм
                        return False  # Возвращаем False (никнейм занят)
        except json.JSONDecodeError:  # Если файл пуст или поврежден
            return True  # Возвращаем True (никнейм свободен)
        except FileNotFoundError as e:  # Если файл не найден
            raise e  # Выбрасываем исключение

        return True  # Возвращаем True (никнейм свободен)

    def get_sorted_stats(self) -> list[dict]:  # Метод для получения отсортированного списка статистики
        """
        Вернуть отсортированный список статов.
        Каждый элемент списка это словарь.
        """
        stats_file = self.__get_stats_file()  # Получаем путь к файлу статистики
        data = []  # Инициализируем список для данных
        try:
            with stats_file.open(mode="r", encoding="utf-8") as rd_f:  # Открываем файл для чтения
                data = json.load(rd_f)  # Загружаем данные из файла
                data.sort(key=lambda dct: dct["Золото"], reverse=True)  # Сортируем данные по количеству золота
        except json.JSONDecodeError:  # Если файл пуст или поврежден
            pass  # Пропускаем исключение
        except FileNotFoundError as e:  # Если файл не найден
            raise e  # Выбрасываем исключение

        return data  # Возвращаем отсортированный список

    def dump_json_stats(self):  # Метод для сохранения текущей статистики в файл
        """
        Сохранить текущие статы в файл.
        Вызывать при завершении игры.
        """
        stats_file = self.__get_stats_file()  # Получаем путь к файлу статистики

        try:
            with stats_file.open(mode="r", encoding="utf-8") as rd_f:  # Открываем файл для чтения
                data = json.load(rd_f)  # Загружаем данные из файла
        except json.JSONDecodeError:  # Если файл пуст или поврежден
            data = []  # Инициализируем пустой список
        except FileNotFoundError as e:  # Если файл не найден
            raise e  # Выбрасываем исключение

        for dct in data:  # Проходим по всем записям
            dct["last_record"] = False  # Сбрасываем флаг последней записи
        data.append(self.form_stats_dict())  # Добавляем текущую статистику в список

        try:
            with stats_file.open(mode="w", encoding="utf-8") as wr_f:  # Открываем файл для записи
                json.dump(data, wr_f, ensure_ascii=False, indent=4)  # Сохраняем данные в файл
        except FileNotFoundError as e:  # Если файл не найден
            raise e  # Выбрасываем исключение

    def __form_current_state_dict(self) -> dict:  # Метод для формирования словаря с текущим состоянием
        from domain.objects.character import Character  # Импортируем класс Character

        return {"nickname": self.nickname, "stats": self._dump(), "character_state": Character.get_instance()._dump()}  # Возвращаем словарь с текущим состоянием

    def dump_json_save(self):  # Метод для сохранения текущего состояния персонажа
        """
        Сохранить текущее состояние персонажа для возможности загрузки.
        """
        save_file = self.__get_save_file()  # Получаем путь к файлу сохранений
        try:
            with save_file.open(mode="w", encoding="utf-8") as wr_f:  # Открываем файл для записи
                json.dump(self.__form_current_state_dict(), wr_f, ensure_ascii=False, indent=4)  # Сохраняем данные в файл
        except Exception as e:  # Если возникает исключение
            raise e  # Выбрасываем исключение

    def load_json_save(self) -> dict:  # Метод для загрузки игры из файла
        """
        Загрузить игру из файла.
        :return: Возвращает словарь с данными для загрузки на персонаже.
        """
        save_file = self.__get_save_file()  # Получаем путь к файлу сохранений
        try:
            with save_file.open(mode="r", encoding="utf-8") as rd_f:  # Открываем файл для чтения
                data = json.load(rd_f)  # Загружаем данные из файла
        except json.JSONDecodeError:  # Если файл пуст или поврежден
            data = {}  # Инициализируем пустой словарь
        except FileNotFoundError as e:  # Если файл не найден
            raise e  # Выбрасываем исключение

        return data  # Возвращаем загруженные данные

    def remove_save(self):  # Метод для удаления файла сохранений
        if not self.__save_dir.exists():  # Если директория для сохранений не существует
            raise FileExistsError(f"Save directory does not exist: {self.__save_dir}")  # Выбрасываем исключение

        for f in self.__save_dir.iterdir():  # Проходим по всем файлам в директории
            if f.name == self.__save_file:  # Если находим файл сохранений
                f.unlink()  # Удаляем файл

    def _dump(self):  # Метод для формирования словаря с текущим состоянием
        return {
            "nickname": self.nickname,  # Никнейм персонажа
            "gold": self.gold,  # Количество золота
            "rogue_level": self.rogue_level,  # Уровень персонажа
            "defeated_enemies": self.defeated_enemies,  # Количество поверженных врагов
            "eaten_food": self.eaten_food,  # Количество съеденной еды
            "used_potions": self.used_potions,  # Количество использованных зелий
            "used_scrolls": self.used_scrolls,  # Количество использованных свитков
            "total_hits": self.total_hits,  # Количество нанесенных ударов
            "missed_hits": self.missed_hits,  # Количество промахов
            "passed_cells": self.passed_cells,  # Количество пройденных клеток
        }

    def _load(self, **kwargs):  # Метод для загрузки состояния из словаря
        self.nickname = kwargs["nickname"]  # Никнейм персонажа
        self.gold = kwargs["gold"]  # Количество золота
        self.rogue_level = kwargs["rogue_level"]  # Уровень персонажа
        self.defeated_enemies = kwargs["defeated_enemies"]  # Количество поверженных врагов
        self.eaten_food = kwargs["eaten_food"]  # Количество съеденной еды
        self.used_potions = kwargs["used_potions"]  # Количество использованных зелий
        self.used_scrolls = kwargs["used_scrolls"]  # Количество использованных свитков
        self.total_hits = kwargs["total_hits"]  # Количество нанесенных ударов
        self.missed_hits = kwargs["missed_hits"]  # Количество промахов
        self.passed_cells = kwargs["passed_cells"]  # Количество пройденных клеток
