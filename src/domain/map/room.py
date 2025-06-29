from dataclasses import dataclass
from random import choice, randint, random, sample
from typing import Any

from domain import Coordinate
from domain.map.corridor import Corridor, Door, Key
from domain.map.settings import (
    ENEMY_CREATION_PROB,
    ITEM_CREATION_PROB,
    ROOM_INDENT,
    ROOM_MINIMUM_HEIGHT,
    ROOM_MINIMUM_WIDTH,
)
from domain.objects.character import Character
from domain.objects.enemies import ENEMIES
from domain.objects.items import ITEMS_GEN
from utils.logger import domain_log

@dataclass(frozen=True, slots=True)
class Exit:
    symbol = "⩎"  # Символ выхода
    color = 30  # Цвет выхода

class Room:
    room_symbol = "."  # Символ комнаты
    room_symbol_color = 1  # Цвет символа комнаты
    room_border_color = 30  # Цвет границы комнаты

    __doors_map = {  # Словарь для определения возможных сторон дверей
        0: ["D", "R"],
        1: ["L", "D", "R"],
        2: ["L", "D"],
        3: ["U", "R", "D"],
        4: ["U", "R", "L", "D"],
        5: ["U", "L", "D"],
        6: ["U", "R"],
        7: ["L", "U", "R"],
        8: ["L", "U"],
    }

    def __init__(self, start_x: int, start_y: int, height: int, width: int, id_: int):
        self.id = id_  # Идентификатор комнаты
        self.x, self.y, self.x_, self.y_ = generate_room(start_x, start_y, height, width)  # Генерация координат комнаты
        self.__objects: dict[Coordinate, Any] = {}  # Словарь для хранения объектов в комнате
        self.__exit: dict[Coordinate, Any] = {}  # Словарь для хранения выхода из комнаты
        self.__items: dict[Coordinate, Any] = {}  # Словарь для хранения предметов в комнате
        self.__keys: dict[Coordinate, Any] = {}  # Словарь для хранения ключей в комнате
        self.__sides = self.__doors_map[self.id].copy()  # Копия списка возможных сторон дверей
        self.__doors_coordinates = set()  # Множество для хранения координат дверей
        self.has_character = False  # Флаг наличия персонажа в комнате
        self.__visited = False  # Флаг посещения комнаты
        self.__doors = []  # Список для хранения дверей
        self.has_keys = []  # Список для хранения ключей
        domain_log.info("{cls} initialized: id={id}", cls=self.__class__.__name__, id=self.id)  # Логирование инициализации комнаты

    @property
    def visited(self):
        return self.__visited  # Возвращает флаг посещения комнаты

    @property
    def doors(self) -> list[Door]:
        return self.__doors  # Возвращает список дверей

    def add_actual_door(self, room: "Room", crd: Coordinate) -> Door:
        door = Door(room, crd)  # Создание двери
        self.__doors.append(door)  # Добавление двери в список
        return door  # Возвращает созданную дверь

    def get_cell(self, y: int, x: int) -> tuple[str, int]:
        """
        Вернуть символ комнаты для координаты.
        """
        if not self.__visited and self.has_character:
            self.__visited = True  # Устанавливает флаг посещения комнаты

        crd = (y, x)  # Координата клетки

        if crd in self.__objects and self.__objects[crd].is_visible:
            return self.__objects[crd].symbol, self.__objects[crd].color  # Возвращает символ и цвет объекта
        if crd in self.__exit:
            return self.__exit[crd].symbol, self.__exit[crd].color  # Возвращает символ и цвет выхода
        if crd in self.__items:
            return self.__items[crd].symbol, self.__items[crd].color  # Возвращает символ и цвет предмета
        if crd in self.__keys:
            return self.__keys[crd].symbol, self.__keys[crd].color  # Возвращает символ и цвет ключа

        return self.room_symbol, self.room_symbol_color  # Возвращает символ и цвет комнаты

    def get_border_symbol(self, y, x) -> tuple[str, int]:
        if (y, x) in self.__doors_coordinates:
            return " ", Corridor.color  # Возвращает символ и цвет коридора

        char = ""  # Символ границы комнаты
        if (x == self.x - 1 or x == self.x_ + 1) and self.y <= y <= self.y_:
            char = "║"  # Вертикальная граница
        elif (y == self.y - 1 or y == self.y_ + 1) and self.x <= x <= self.x_:
            char = "═"  # Горизонтальная граница
        elif y == self.y - 1 and x == self.x - 1:
            char = "╔"  # Левый верхний угол
        elif x == self.x - 1 and y == self.y_ + 1:
            char = "╚"  # Левый нижний угол
        elif x == self.x_ + 1 and y == self.y - 1:
            char = "╗"  # Правый верхний угол
        elif x == self.x_ + 1 and y == self.y_ + 1:
            char = "╝"  # Правый нижний угол

        return char, self.room_border_color  # Возвращает символ и цвет границы комнаты

    def is_in(self, crd: Coordinate) -> bool:
        return self.y <= crd[0] <= self.y_ and self.x <= crd[1] <= self.x_  # Проверяет, находится ли координата внутри комнаты

    def is_in_and_available_for_move(self, crd: Coordinate) -> bool:
        return self.y <= crd[0] <= self.y_ and self.x <= crd[1] <= self.x_ and crd not in self.__objects  # Проверяет, находится ли координата внутри комнаты и доступна для перемещения

    def is_in_and_available(self, crd: Coordinate) -> bool:
        return (
            self.y <= crd[0] <= self.y_
            and self.x <= crd[1] <= self.x_
            and crd not in self.__objects
            and crd not in self.__items
        )  # Проверяет, находится ли координата внутри комнаты и доступна

    def place_character(self):
        crds = randint(self.y, self.y_), randint(self.x, self.x_)  # Генерация случайных координат для персонажа
        self.__objects[crds] = Character.get_instance()  # Размещение персонажа в комнате
        Character.get_instance().place(crds)  # Установка координат персонажа
        domain_log.info("Character is placed to room id={id}", id=self.id)  # Логирование размещения персонажа
        self.has_character = True  # Установка флага наличия персонажа в комнате
        self.__visited = True  # Установка флага посещения комнаты

    def place_exit(self):
        self.__exit[randint(self.y, self.y_), randint(self.x, self.x_)] = Exit()  # Размещение выхода в комнате
        domain_log.info("Exit is placed to room id={id}", id=self.id)  # Логирование размещения выхода

    def generate_enemies(self, level: int, coef: float):
        """
        Сгенерировать врагов в комнате.
        Количество врагов зависит от уровня и коэффициента.
        :param level: Уровень игры от 1 до 21
        :param coef: [0, 2], 1 = не изменять стандартный шанс на генерацию
        """
        for _ in range(level // 4 + 2):
            if random() - ((level * coef) / 100) < ENEMY_CREATION_PROB:
                domain_log.info("Generating enemy in room id={id}", id=self.id)  # Логирование генерации врага
                self.__objects[self.__allocate_coordinates()] = choice(ENEMIES)(level)  # Генерация врага

    def generate_items(self, level: int, coef: float):
        """
        Сгенерировать предметы в комнате.
        Количество предметов зависит от уровня и коэффициента.
        :param level: Уровень игры от 1 до 21
        :param coef: [0, 2], 1 = не изменять стандартный шанс на генерацию
        """
        coef = 0 if coef == 1 else coef
        coef = coef - 1 if coef > 1 else (1 - coef) * -1
        coef = (20 * coef) / 100
        if self.__objects:
            for _ in range(len(self.__objects) - self.has_character):
                if random() - coef + level / 100 < ITEM_CREATION_PROB:
                    domain_log.info("Generating item in room id={id}", id=self.id)  # Логирование генерации предмета
                    self.__items[self.__allocate_coordinates()] = choice(ITEMS_GEN)(level)  # Генерация предмета
        elif random() - coef + level / 100 < ITEM_CREATION_PROB:
            domain_log.info("Generating item in room id={id}", id=self.id)  # Логирование генерации предмета
            self.__items[self.__allocate_coordinates()] = choice(ITEMS_GEN)(level)  # Генерация предмета

    def generate_keys(self):
        for key in self.has_keys:
            self.__keys[self.__allocate_coordinates()] = Key(key)  # Генерация ключа

    def get_random_crd_in_zone(self, crd: Coordinate, radius: int) -> Coordinate:
        """
        Получить рандомную координату в зоне с радиусом
        :param crd: начальная координата
        :param radius: радиус, если радиус == 0 выбрать всю доступную область
        """
        if not radius:
            return self.__allocate_coordinates()  # Возвращает случайные координаты в комнате

        y, x = crd
        x_ = randint(max(self.x, x - radius), min(self.x_, x + radius))
        y_ = randint(max(self.y, y - radius), min(self.y_, y + radius))
        while (y_, x_) in (self.__objects | self.__items | self.__keys):
            x_ = randint(max(self.x, x - radius), min(self.x_, x + radius))
            y_ = randint(max(self.y, y - radius), min(self.y_, y + radius))
        return y_, x_  # Возвращает случайные координаты в зоне

    def __allocate_coordinates(self) -> Coordinate:
        x_ = randint(self.x, self.x_)
        y_ = randint(self.y, self.y_)
        while (y_, x_) in (self.__objects | self.__exit | self.__items | self.__keys):
            x_ = randint(self.x, self.x_)
            y_ = randint(self.y, self.y_)
        return y_, x_  # Возвращает случайные координаты в комнате

    def random_door_sides(self) -> list[str]:
        return sample(self.__sides, k=randint(1, len(self.__sides))) if self.__sides else self.__sides  # Возвращает случайные стороны для дверей

    def __remove_side(self, side: str):
        if side in self.__sides:
            self.__sides.remove(side)  # Удаляет сторону из списка возможных сторон дверей

    def generate_door(self, side: str) -> tuple[int, str, Coordinate]:
        self.__remove_side(side)  # Удаляет сторону из списка возможных сторон дверей
        match side:
            case "U":
                crd = self.y - 1, randint(self.x + 1, self.x_ - 1)
                self.__doors_coordinates.add(crd)
                return self.id - 3, "D", crd  # Возвращает идентификатор соседней комнаты, сторону и координаты двери
            case "D":
                crd = self.y_ + 1, randint(self.x + 1, self.x_ - 1)
                self.__doors_coordinates.add(crd)
                return self.id + 3, "U", crd  # Возвращает идентификатор соседней комнаты, сторону и координаты двери
            case "L":
                crd = randint(self.y + 1, self.y_ - 1), self.x - 1
                self.__doors_coordinates.add(crd)
                return self.id - 1, "R", crd  # Возвращает идентификатор соседней комнаты, сторону и координаты двери
            case "R":
                crd = randint(self.y + 1, self.y_ - 1), self.x_ + 1
                self.__doors_coordinates.add(crd)
                return self.id + 1, "L", crd  # Возвращает идентификатор соседней комнаты, сторону и координаты двери
            case _:
                raise ValueError(f"Unable to generate door with an unknown side {side}")  # Выбрасывает исключение при неизвестной стороне

    def add_object(self, crd: Coordinate, obj: Any):
        self.__objects[crd] = obj  # Добавляет объект в комнату

    def remove_object(self, crd: Coordinate):
        del self.__objects[crd]  # Удаляет объект из комнаты

    def get_object(self, crd: Coordinate) -> Any | None:
        """
        Получить объект комнаты по координате
        :return: объект или none
        """
        return self.__objects.get(crd)  # Возвращает объект или None, если объекта нет

    @property
    def objects(self):
        return self.__objects.copy()  # Возвращает копию словаря объектов

    def get_exit(self, crd: Coordinate) -> Any | None:
        """
        Получить объект комнаты по координате
        :return: объект или none
        """
        return self.__exit.get(crd)  # Возвращает выход или None, если выхода нет

    def get_key(self, crd: Coordinate) -> Any | None:
        if key := self.__keys.get(crd):
            Character.get_instance().keys.append(key.color - 3)  # Добавляет ключ персонажу
            self.__keys.pop(crd)  # Удаляет ключ из комнаты
            return key  # Возвращает ключ
        return None  # Возвращает None, если ключа нет

    def face_door(self, crd: Coordinate):
        if crd in self.__doors_coordinates:
            door = [d for d in self.doors if d.crd == crd]
            if door and door[0].is_closed:
                return door[0]  # Возвращает дверь, если она закрыта
        return None  # Возвращает None, если дверь не найдена или открыта

    def add_item(self, crd: Coordinate, obj: Any):
        self.__items[crd] = obj  # Добавляет предмет в комнату

    def remove_item(self, crd: Coordinate):
        del self.__items[crd]  # Удаляет предмет из комнаты

    def get_item(self, crd: Coordinate) -> Any | None:
        """
        Получить объект комнаты по координате
        :return: объект или none
        """
        return self.__items.get(crd, None)  # Возвращает предмет или None, если предмета нет

    def is_exit(self, crd: Coordinate) -> bool:
        return crd in self.__exit  # Возвращает True, если координата является выходом

def generate_room(x: int, y: int, height: int, width: int) -> tuple[int, int, int, int]:
    """
    Сгенерировать комнату внутри заданного прямоугольника.
    :param x: Координата колонки
    :param y: Координата строки
    :param height: Длина стороны y
    :param width: Длина стороны x
    :return: Координаты комнаты (лев верх, прав ниж)
    """
    if height - ROOM_INDENT * 2 < ROOM_MINIMUM_HEIGHT or width - ROOM_INDENT * 2 < ROOM_MINIMUM_WIDTH:
        raise ValueError("Unable to create room with insufficient space")  # Выбрасывает исключение при недостаточном пространстве

    r_width = randint(ROOM_MINIMUM_WIDTH, width - ROOM_INDENT * 2)
    r_height = randint(ROOM_MINIMUM_HEIGHT, height - ROOM_INDENT * 2)

    room_x = randint(x + ROOM_INDENT, x + width - ROOM_INDENT - r_width)
    room_y = randint(y + ROOM_INDENT, y + height - ROOM_INDENT - r_height)

    return room_x, room_y, room_x + r_width - 1, room_y + r_height - 1  # Возвращает координаты комнаты
