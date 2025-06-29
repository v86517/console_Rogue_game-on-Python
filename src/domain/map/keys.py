# Импортируем модуль Enum для создания перечислений
from enum import Enum
# Импортируем функции choice и sample из модуля random для случайного выбора элементов
from random import choice, sample

# Импортируем классы Door, Room и Character из соответствующих модулей
from domain.map.corridor import Door
from domain.map.room import Room
from domain.objects.character import Character

# Создаем перечисление DoorsColor с тремя цветами дверей
class DoorsColor(Enum):
    RED = 16
    GREEN = 17
    BLUE = 18

# Функция для генерации закрытых дверей на уровне
def generate_locked_doors(level_map):
    # Очищаем список ключей у персонажа
    Character.get_instance().keys = []
    # Определяем доступные цвета дверей
    colors = (DoorsColor.RED, DoorsColor.GREEN, DoorsColor.BLUE)
    # Определяем количество ключей
    keys_amt = len(colors)

    # Создаем список всех дверей на уровне
    all_doors: list[Door] = []
    for room in level_map.rooms:
        all_doors.extend(room.doors)
        # Находим комнату, в которой находится персонаж
        if room.has_character:
            start_room = room
    # Выбираем случайные двери для закрытия
    locked_doors = sample(all_doors, keys_amt)
    # Закрываем выбранные двери
    lock_doors(locked_doors, colors)

    # Распределяем ключи по комнатам
    for _ in range(keys_amt):
        keys, available_rooms = [], []
        get_available_rooms(keys, available_rooms, start_room)
        key = keys.pop()
        place_key(available_rooms, key)
        # Открываем двери, соответствующие найденным ключам
        for door in locked_doors:
            if door.color == key.value:
                door.lock = False
                door.color = Door.base_color

    # Закрываем двери снова после распределения ключей
    lock_doors(locked_doors, colors)

# Функция для закрытия дверей
def lock_doors(doors_to_lock, colors):
    for i, door in enumerate(doors_to_lock):
        door.lock = True
        door.color = colors[i].value

# Функция для размещения ключа в одной из доступных комнат
def place_key(available_rooms: list[Room], key: DoorsColor):
    room = choice(available_rooms)
    room.has_keys.append(key.value)

# Функция для получения списка доступных комнат
def get_available_rooms(keys: list[DoorsColor], available_rooms: list[Room], room: Room):
    available_rooms.append(room)
    for door in room.doors:
        door_ = get_2nd_door(room, door)
        closed_door = get_closed_door(door, door_)
        if not closed_door and door.room not in available_rooms:
            get_available_rooms(keys, available_rooms, door.room)
        elif closed_door and DoorsColor(closed_door.color) not in keys:
            keys.append(DoorsColor(door.color if door.is_closed else door_.color))

# Функция для получения второй двери, соединяющей две комнаты
def get_2nd_door(room: Room, door_: Door):
    for door in door_.room.doors:
        if door.room == room:
            return door
    return None

# Функция для получения закрытой двери из двух дверей
def get_closed_door(door1: Door, door2: Door) -> None | Door:
    if door1.is_closed:
        return door1
    if door2.is_closed:
        return door2
    return None
