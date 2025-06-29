from random import choice, randint  # Импортируем функции choice и randint из модуля random для генерации случайных чисел
from typing import Any  # Импортируем тип Any из модуля typing для аннотации типов

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage  # Импортируем классы и перечисления из модуля sound_controller
from domain import Coordinate  # Импортируем класс Coordinate из модуля domain
from domain.objects.character import Character  # Импортируем класс Character из модуля domain.objects.character
from domain.objects.enemies.enemy import Enemy  # Импортируем класс Enemy из модуля domain.objects.enemies.enemy
from domain.objects.utils import IS_MACOS  # Импортируем константу IS_MACOS из модуля domain.objects.utils
from utils.logger import domain_log  # Импортируем логгер domain_log из модуля utils.logger

class Key:  # Определяем класс Key для представления ключа
    symbol = "\u0482" if IS_MACOS else "⚿"  # Символ ключа в зависимости от операционной системы
    __info_map = {  # Словарь для сопоставления цвета ключа с его описанием
        16: "красный",
        17: "зеленый",
        18: "синий",
    }

    def __init__(self, color):  # Конструктор класса Key
        self.color = color + 3  # Устанавливаем цвет ключа
        self.info = self.__info_map[color]  # Получаем описание цвета ключа
        self.add_sound = SoundController.get_instance().get_sound(SoundType.Key, SoundUsage.add)  # Звук добавления ключа

class Door:  # Определяем класс Door для представления двери
    base_symbol = "%"  # Базовый символ двери
    base_color = 6  # Базовый цвет двери

    def __init__(self, room, crd: Coordinate):  # Конструктор класса Door
        self.room = room  # Комната, к которой принадлежит дверь
        self.crd = crd  # Координаты двери
        self.symbol = self.base_symbol  # Символ двери
        self.color = self.base_color  # Цвет двери
        self.lock = False  # Флаг закрытия двери
        self.closed_sound = SoundController.get_instance().get_sound(SoundType.Door, SoundUsage.closed)  # Звук закрытия двери
        self.open_sound = SoundController.get_instance().get_sound(SoundType.Door, SoundUsage.open)  # Звук открытия двери

    @property
    def is_open(self):  # Свойство для проверки, открыта ли дверь
        return not self.lock  # Возвращаем True, если дверь открыта

    @property
    def is_closed(self):  # Свойство для проверки, закрыта ли дверь
        return self.lock  # Возвращаем True, если дверь закрыта

class Corridor:  # Определяем класс Corridor для представления коридора
    corridor_symbol = "."  # Символ коридора
    empty_symbol = " "  # Символ пустой клетки
    color = 6  # Цвет коридора

    def __init__(self, start_door: Door, finish_door: Door, direction: str):  # Конструктор класса Corridor
        """
        Начало и конец коридора - это двери в границе комнаты.
        Для генерации оставшегося коридора двери обрезаются.
        :param direction: 'v' (vertical) | 'h' (horizontal)
        """
        self.__doors = {  # Словарь для хранения дверей коридора
            start_door.crd: start_door,
            finish_door.crd: finish_door,
        }
        self.__corridor: dict[Coordinate, Any] = {}  # Словарь для хранения клеток коридора
        self.__objects: dict[Coordinate, Any] = {}  # Словарь для хранения объектов в коридоре
        self.__items: dict[Coordinate, Any] = {}  # Словарь для хранения предметов в коридоре
        self.start, self.finish = self.__shift_initials(start_door.crd, finish_door.crd, direction)  # Сдвигаем начальные координаты дверей
        self.__generate_corridor(direction)  # Генерируем коридор
        self.has_character = False  # Флаг наличия персонажа в коридоре
        self.__visited = False  # Флаг посещения коридора
        domain_log.info("{cls} initialized", cls=self.__class__.__name__)  # Логируем инициализацию коридора

    @property
    def doors(self):  # Свойство для получения словаря дверей
        return self.__doors  # Возвращаем словарь дверей

    @staticmethod
    def __shift_initials(start: Coordinate, finish: Coordinate, direction: str) -> tuple[Coordinate, Coordinate]:  # Метод для сдвига начальных координат дверей
        y, x = start  # Начальные координаты первой двери
        y_, x_ = finish  # Конечные координаты второй двери
        if direction == "v":  # Если направление вертикальное
            if y > y_:  # Если начальная координата выше конечной
                new_s, new_f = (y - 1, x), (y_ + 1, x_)  # Сдвигаем координаты вниз
            else:  # Если начальная координата ниже конечной
                new_s, new_f = (y + 1, x), (y_ - 1, x_)  # Сдвигаем координаты вверх
        elif direction == "h":  # Если направление горизонтальное
            if x > x_:  # Если начальная координата правее конечной
                new_s, new_f = (y, x - 1), (y_, x_ + 1)  # Сдвигаем координаты влево
            else:  # Если начальная координата левее конечной
                new_s, new_f = (y, x + 1), (y_, x_ - 1)  # Сдвигаем координаты вправо
        else:  # Если направление неверное
            raise ValueError("direction must be 'v' or 'h'")  # Выбрасываем исключение

        return new_s, new_f  # Возвращаем новые координаты

    def __generate_corridor(self, direction: str):  # Метод для генерации коридора
        y, x = self.start  # Начальные координаты коридора
        y_, x_ = self.finish  # Конечные координаты коридора
        if direction == "v":  # Если направление вертикальное
            domain_log.info("Generating vertical corridor")  # Логируем генерацию вертикального коридора
            if x == x_:  # Если координаты по x совпадают
                self.__build_vertical_corridor(y, y_, x)  # Строим вертикальный коридор
            else:  # Если координаты по x не совпадают
                y_turn = randint(min(y, y_), max(y, y_))  # Генерируем случайную координату поворота по y
                self.__build_vertical_corridor(y, y_turn, x)  # Строим вертикальный коридор до поворота
                self.__build_horizontal_corridor(x, x_, y_turn)  # Строим горизонтальный коридор до поворота
                self.__build_vertical_corridor(y_, y_turn, x_)  # Строим вертикальный коридор после поворота
        elif direction == "h":  # Если направление горизонтальное
            domain_log.info("Generating horizontal corridor")  # Логируем генерацию горизонтального коридора
            if y == y_:  # Если координаты по y совпадают
                self.__build_horizontal_corridor(x, x_, y)  # Строим горизонтальный коридор
            else:  # Если координаты по y не совпадают
                x_turn = randint(min(x, x_), max(x, x_))  # Генерируем случайную координату поворота по x
                self.__build_horizontal_corridor(x, x_turn, y)  # Строим горизонтальный коридор до поворота
                self.__build_vertical_corridor(y, y_, x_turn)  # Строим вертикальный коридор до поворота
                self.__build_horizontal_corridor(x_, x_turn, y_)  # Строим горизонтальный коридор после поворота

    def __build_vertical_corridor(self, y1: int, y2: int, x: int):  # Метод для построения вертикального коридора
        for y in range(min(y1, y2), max(y1, y2) + 1):  # Проходим по всем координатам по y
            self.__corridor[y, x] = self.corridor_symbol  # Устанавливаем символ коридора

    def __build_horizontal_corridor(self, x1: int, x2: int, y: int):  # Метод для построения горизонтального коридора
        for x in range(min(x1, x2), max(x1, x2) + 1):  # Проходим по всем координатам по x
            self.__corridor[y, x] = self.corridor_symbol  # Устанавливаем символ коридора

    def get_cell(self, y: int, x: int) -> tuple[str, int]:  # Метод для получения символа и цвета клетки коридора
        """
        Вернуть символ коридора для координаты.
        """
        crd = (y, x)  # Координата клетки
        if crd in self.__objects and self.__objects[crd].is_visible:  # Если в клетке есть видимый объект
            return self.__objects[crd].symbol, self.__objects[crd].color  # Возвращаем символ и цвет объекта
        if crd in self.__items:  # Если в клетке есть предмет
            return self.__items[crd].symbol, self.__items[crd].color  # Возвращаем символ и цвет предмета
        if crd in self.__doors:  # Если в клетке есть дверь
            return self.__doors[crd].symbol, self.__doors[crd].color  # Возвращаем символ и цвет двери

        return self.__corridor[crd], self.color  # Возвращаем символ и цвет коридора

    def is_in(self, crd: Coordinate) -> bool:  # Метод для проверки, находится ли координата в коридоре
        return crd in self.__corridor or (crd in self.__doors and self.__doors[crd].is_open)  # Возвращаем True, если координата в коридоре или в открытой двери

    def is_in_visible(self, crd: Coordinate) -> bool:  # Метод для проверки, видна ли координата в коридоре
        return crd in self.__corridor or crd in self.__doors  # Возвращаем True, если координата в коридоре или в двери

    def is_in_and_available_for_move(self, crd: Coordinate) -> bool:  # Метод для проверки, доступна ли координата для перемещения
        return (
            crd in self.__corridor or (crd in self.__doors and self.__doors[crd].is_open)
        ) and crd not in self.__objects  # Возвращаем True, если координата в коридоре или в открытой двери и нет объектов

    def is_in_and_available(self, crd: Coordinate) -> bool:  # Метод для проверки, доступна ли координата
        return (crd in self.__corridor or crd in self.__doors) and crd not in self.__objects and crd not in self.__items  # Возвращаем True, если координата в коридоре или в двери и нет объектов и предметов

    def get_random_crd_in_zone(self, crd: Coordinate, radius: int) -> Coordinate:  # Метод для получения случайной координаты в зоне
        """
        Получить рандомную координату в зоне с радиусом
        :param crd: начальная координата
        :param radius: радиус, если радиус == 0 выбрать всю доступную область
        """
        if not radius:  # Если радиус равен 0
            y_, x_ = choice(list(self.__corridor))  # Выбираем случайную координату из коридора
            while (y_, x_) in self.__objects or (y_, x_) in self.__items:  # Пока в координате есть объекты или предметы
                y_, x_ = choice(list(self.__corridor))  # Выбираем новую случайную координату
            return y_, x_  # Возвращаем случайную координату

        y, x = crd  # Начальная координата
        available_cord = []  # Список доступных координат
        for cor_crd in self.__corridor:  # Проходим по всем координатам коридора
            if cor_crd in self.__objects or cor_crd in self.__items:  # Если в координате есть объекты или предметы
                continue  # Пропускаем координату
            if y - radius <= cor_crd[0] <= y + radius and x - radius <= cor_crd[1] <= x + radius:  # Если координата в зоне радиуса
                available_cord.append(cor_crd)  # Добавляем координату в список доступных
        return choice(available_cord)  # Возвращаем случайную координату из списка доступных

    def add_object(self, crd: Coordinate, obj: Character | Enemy):  # Метод для добавления объекта в коридор
        self.__objects[crd] = obj  # Добавляем объект в словарь объектов

    def remove_object(self, crd: Coordinate):  # Метод для удаления объекта из коридора
        del self.__objects[crd]  # Удаляем объект из словаря объектов

    def get_object(self, crd: Coordinate) -> Character | Enemy | None:  # Метод для получения объекта коридора по координате
        """
        Получить объект коридора по координате
        :return: объект или none
        """
        return self.__objects.get(crd)  # Возвращаем объект или None, если объекта нет

    @property
    def objects(self):  # Свойство для получения копии словаря объектов
        return self.__objects.copy()  # Возвращаем копию словаря объектов

    def add_item(self, crd: Coordinate, obj: Any):  # Метод для добавления предмета в коридор
        self.__items[crd] = obj  # Добавляем предмет в словарь предметов

    def remove_item(self, crd: Coordinate):  # Метод для удаления предмета из коридора
        del self.__items[crd]  # Удаляем предмет из словаря предметов

    def get_item(self, crd: Coordinate) -> Any | None:  # Метод для получения предмета коридора по координате
        """
        Получить объект комнаты по координате
        :return: объект или none
        """
        return self.__items.get(crd)  # Возвращаем предмет или None, если предмета нет

    def face_door(self, crd: Coordinate):  # Метод для получения двери по координате, если дверь закрыта
        if crd in self.__doors and self.__doors[crd].is_closed:  # Если координата в двери и дверь закрыта
            return self.__doors[crd]  # Возвращаем дверь
        return None  # Возвращаем None, если дверь не найдена или открыта
