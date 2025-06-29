from random import choice  # Импортируем функцию choice из модуля random для случайного выбора
from time import sleep  # Импортируем функцию sleep из модуля time для задержек

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage  # Импортируем классы и перечисления из модуля sound_controller
from datalayer.stats import RogueStats  # Импортируем класс RogueStats из модуля datalayer.stats
from domain import Coordinate  # Импортируем класс Coordinate из модуля domain
from domain.map.corridor import Corridor, Door  # Импортируем классы Corridor и Door из модуля domain.map.corridor
from domain.map.keys import generate_locked_doors  # Импортируем функцию generate_locked_doors из модуля domain.map.keys
from domain.map.room import Room  # Импортируем класс Room из модуля domain.map.room
from domain.objects.character import Character  # Импортируем класс Character из модуля domain.objects.character
from domain.objects.enemies.enemy import Enemy  # Импортируем класс Enemy из модуля domain.objects.enemies.enemy
from domain.objects.items.item import Item  # Импортируем класс Item из модуля domain.objects.items.item
from domain.objects.utils import MovePattern, RogueEvent  # Импортируем перечисления MovePattern и RogueEvent из модуля domain.objects.utils
from utils.logger import domain_log  # Импортируем логгер domain_log из модуля utils.logger

class LevelMap:
    """
    Класс - карта уровня.
    При инициализации генерирует 9 комнат и случайные коридоры.
    """

    __map_symbol = " "  # Символ для пустой клетки карты
    __map_color = 1  # Цвет для пустой клетки карты
    __corridor_color = Corridor.color  # Цвет для коридора

    def __init__(self, height: int, width: int, level: int, complexity_coef: float):
        """
        Сгенерировать карту уровня.
        :param level: Уровень игры от 1 до 21
        :param complexity_coef: сложность [0, 2], где 1 = не изменять стандартный шанс на генерацию
        """
        self.height = height  # Высота карты
        self.width = width  # Ширина карты
        self.y, self.x, self.y_, self.x_ = 0, 0, self.height - 1, self.width - 1  # Границы карты
        self.__character = self.__get_character()  # Получаем экземпляр персонажа
        self.__rooms = self.__generate_level_rooms()  # Генерируем комнаты уровня
        self.__corridors: list[Corridor] = []  # Список для хранения коридоров
        self.__generate_doors_and_corridors()  # Генерируем двери и коридоры
        self.__place_character_to_initial_room()  # Размещаем персонажа в начальной комнате
        generate_locked_doors(self)  # Генерируем закрытые двери
        self.__place_exit()  # Размещаем выход
        self.__generate_enemies(level, complexity_coef)  # Генерируем врагов
        self.__generate_items(level, complexity_coef)  # Генерируем предметы
        self.__visited_corridors = set()  # Множество для хранения посещенных коридоров
        domain_log.info("{cls} initialized", cls=self.__class__.__name__)  # Логируем инициализацию карты уровня
        self.__sound = SoundController.get_instance().get_sound(SoundType.Level, SoundUsage.open)  # Получаем звук открытия уровня

        if level > 1:  # Если уровень больше 1
            self.__sound.play()  # Воспроизводим звук открытия уровня

    @property
    def rooms(self):
        return self.__rooms  # Возвращаем список комнат

    def __get_character(self) -> Character:
        ch = Character.get_instance()  # Получаем экземпляр персонажа
        if not ch:  # Если персонаж не существует
            domain_log.error("{cls} failed to get character", cls=self.__class__.__name__)  # Логируем ошибку
            raise AttributeError("Please create a character before LevelMap initializing.")  # Выбрасываем исключение
        return ch  # Возвращаем экземпляр персонажа

    def __generate_level_rooms(self) -> list[Room]:
        rooms = []  # Список для хранения комнат
        w_size = self.width // 3  # Ширина комнаты
        h_size = self.height // 3  # Высота комнаты
        w_step = w_size - 1  # Шаг по ширине
        h_step = h_size - 1  # Шаг по высоте

        room_id = 0  # Идентификатор комнаты
        for col in range(3):  # Проходим по столбцам
            for row in range(3):  # Проходим по строкам
                rooms.append(Room(self.x + w_step * row, self.y + h_step * col, h_size, w_size, room_id))  # Добавляем комнату в список
                room_id += 1  # Увеличиваем идентификатор комнаты

        return rooms  # Возвращаем список комнат

    def __place_character_to_initial_room(self):
        choice(self.__rooms).place_character()  # Размещаем персонажа в случайной комнате

    def __place_exit(self):
        choice(list(filter(lambda r: not r.has_character, self.__rooms))).place_exit()  # Размещаем выход в случайной комнате без персонажа

    def __generate_doors_and_corridors(self):
        room_id = 0  # Идентификатор комнаты

        room_groups = [set()]  # Список для хранения групп комнат
        cur_group = 0  # Текущая группа

        def gen_doors_and_corridor(id_: int, side_: str) -> int:
            next_id_, next_busy_side, start_coord = self.__rooms[id_].generate_door(side_)  # Генерируем дверь
            start_door = self.__rooms[id_].add_actual_door(self.__rooms[next_id_], start_coord)  # Добавляем дверь в комнату
            _, _, finish_coord = self.__rooms[next_id_].generate_door(next_busy_side)  # Генерируем дверь в соседней комнате
            finish_door = self.__rooms[next_id_].add_actual_door(self.__rooms[id_], finish_coord)  # Добавляем дверь в соседнюю комнату
            domain_log.info(
                "Generating a new corridor: room_id={id}, next_id={n_id}, initial_side={side}",
                id=id_,
                n_id=next_id_,
                side=side_,
            )  # Логируем генерацию нового коридора
            self.__corridors.append(Corridor(start_door, finish_door, direction="v" if side_ in ["U", "D"] else "h"))  # Добавляем коридор в список
            return next_id_  # Возвращаем идентификатор следующей комнаты

        def union_groups():
            nonlocal room_groups  # Используем nonlocal для изменения переменной room_groups
            for i in range(len(room_groups) - 1):  # Проходим по группам комнат
                for j in range(i + 1, len(room_groups)):  # Проходим по оставшимся группам
                    if room_groups[i] & room_groups[j]:  # Если группы пересекаются
                        room_groups[i] |= room_groups[j]  # Объединяем группы
                        domain_log.info("Corridor groups are united")  # Логируем объединение групп

        def check_connections() -> bool:
            nonlocal room_groups  # Используем nonlocal для изменения переменной room_groups
            return all(room_groups[0] & room_groups[i] for i in range(1, len(room_groups)))  # Проверяем, что все группы соединены

        def add_connection():
            nonlocal room_groups  # Используем nonlocal для изменения переменной room_groups
            for i in range(1, len(room_groups)):  # Проходим по группам комнат
                if not room_groups[0] & room_groups[i]:  # Если группа не соединена с первой группой
                    id_ = choice(list(room_groups[i]))  # Выбираем случайную комнату из группы
                    domain_log.warning("Adding new connection for room {id}", id=id_)  # Логируем добавление нового соединения
                    for door in self.__rooms[id_].random_door_sides():  # Проходим по сторонам двери
                        room_groups[i].add(gen_doors_and_corridor(id_, door))  # Добавляем комнату в группу

        visited_rooms = [False for _ in self.__rooms]  # Список для отслеживания посещенных комнат
        while not all(visited_rooms):  # Пока есть непосещенные комнаты
            visited_rooms[room_id] = True  # Отмечаем комнату как посещенную
            next_id = -1  # Идентификатор следующей комнаты
            for side in self.__rooms[room_id].random_door_sides():  # Проходим по сторонам двери
                next_id = gen_doors_and_corridor(room_id, side)  # Генерируем дверь и коридор
                room_groups[cur_group].add(room_id)  # Добавляем комнату в текущую группу
                visited_rooms[next_id] = True  # Отмечаем следующую комнату как посещенную
                room_groups[cur_group].add(next_id)  # Добавляем следующую комнату в текущую группу

            if next_id == -1:  # Если не удалось сгенерировать соединение
                domain_log.warning("Random doors generation: adding new group")  # Логируем добавление новой группы
                room_id = visited_rooms.index(False)  # Выбираем случайную непосещенную комнату
                room_groups.append(set())  # Добавляем новую группу
                cur_group += 1  # Увеличиваем текущую группу
                continue  # Продолжаем генерацию

            room_id = next_id  # Устанавливаем идентификатор следующей комнаты

        union_groups()  # Объединяем группы комнат
        while not check_connections():  # Пока есть несоединенные группы
            domain_log.warning("Found a rooms group without direct connection")  # Логируем наличие несоединенной группы
            add_connection()  # Добавляем соединение
            union_groups()  # Объединяем группы комнат

    def __generate_enemies(self, level: int, coef: float):
        """
        Сгенерировать противников во всех комнатах.
        :param level: Уровень игры от 1 до 21
        :param coef: [0, 2], где 1 = не изменять стандартный шанс на генерацию
        """
        for room in self.__rooms:  # Проходим по комнатам
            if room.has_character:  # Если в комнате есть персонаж
                continue  # Пропускаем комнату
            room.generate_enemies(level, coef)  # Генерируем врагов в комнате

    def __generate_items(self, level: int, coef: float):
        """
        Сгенерировать предметы во всех комнатах.
        :param level: Уровень игры от 1 до 21
        :param coef: [0, 2], где 1 = не изменять стандартный шанс на генерацию
        """
        for room in self.__rooms:  # Проходим по комнатам
            room.generate_items(level, coef)  # Генерируем предметы в комнате
            room.generate_keys()  # Генерируем ключи в комнате

    def move_character(self, direction: str) -> list[RogueEvent]:
        events, able_to_move = self.__character.check_object_effects()  # Проверяем эффекты объектов на персонаже
        if not able_to_move:  # Если персонаж не может двигаться
            return events  # Возвращаем события

        y, x = self.__character.get_crd()  # Получаем координаты персонажа
        match direction:  # Определяем направление движения
            case "w":
                y -= 1  # Движение вверх
            case "s":
                y += 1  # Движение вниз
            case "a":
                x -= 1  # Движение влево
            case "d":
                x += 1  # Движение вправо
            case _:
                raise ValueError(f"Invalid direction {direction}")  # Выбрасываем исключение при неверном направлении

        domain_log.info("Moving character: {d}", d=direction)  # Логируем движение персонажа
        events.extend(self.__move_character((y, x)))  # Двигаем персонажа

        return events  # Возвращаем события

    def __move_character(self, crd: Coordinate) -> list[RogueEvent]:
        for place in self.__rooms + self.__corridors:  # Проходим по комнатам и коридорам
            if place.is_in(crd) or place.face_door(crd):  # Если координата находится в комнате или коридоре или перед дверью
                domain_log.info(f"Moving character in {place.__class__.__name__}")  # Логируем движение персонажа
                return self.__move_actor(place, crd)  # Двигаем персонажа

        return []  # Возвращаем пустой список событий

    def __move_actor(self, place: Room | Corridor, crd: Coordinate) -> list[RogueEvent]:
        events = []  # Список для хранения событий
        if place.get_object(crd):  # Если в координате есть объект
            domain_log.info("Character faced an enemy")  # Логируем встречу с врагом
            events.extend(self.__attack_enemy(place, crd))  # Атакуем врага
            return events  # Возвращаем события

        if door := place.face_door(crd):  # Если в координате есть дверь
            events.append(self.__knock_the_door(door))  # Взаимодействуем с дверью
            return events  # Возвращаем события

        if isinstance(place, Room) and place.get_exit(crd):  # Если в координате есть выход
            events.append(RogueEvent("Вы нашли выход"))  # Добавляем событие нахождения выхода

        if place.get_item(crd):  # Если в координате есть предмет
            domain_log.info("Character found item")  # Логируем нахождение предмета
            events.extend(self.__pick_up_item(place, crd))  # Подбираем предмет

        if isinstance(place, Room) and (key := place.get_key(crd)):  # Если в координате есть ключ
            key.add_sound.play()  # Воспроизводим звук подбора ключа
            events.append(RogueEvent(f"Вы нашли {key.info} ключ", key.color))  # Добавляем событие нахождения ключа

        self.__remove_character()  # Удаляем персонажа с текущей позиции
        place.has_character = True  # Устанавливаем флаг наличия персонажа в комнате или коридоре
        place.add_object(crd, self.__character)  # Добавляем персонажа в новую позицию
        self.__character.place(crd)  # Устанавливаем новые координаты персонажа
        RogueStats.get_instance().passed_cells += 1  # Увеличиваем количество пройденных клеток
        return events  # Возвращаем события

    def __knock_the_door(self, door: Door):
        character = Character.get_instance()  # Получаем экземпляр персонажа
        if door.color in character.keys:  # Если у персонажа есть ключ от двери
            door.lock = False  # Открываем дверь
            character.keys.remove(door.color)  # Удаляем ключ из инвентаря персонажа
            door.color = Door.base_color  # Устанавливаем базовый цвет двери
            door.open_sound.play()  # Воспроизводим звук открытия двери
            return RogueEvent("Вы открыли дверь с помощью ключа")  # Возвращаем событие открытия двери
        door.closed_sound.play()  # Воспроизводим звук закрытия двери
        return RogueEvent("Дверь заперта. Найдите подходящий ключ")  # Возвращаем событие закрытия двери

    def __remove_character(self):
        for obj in self.__rooms + self.__corridors:  # Проходим по комнатам и коридорам
            if obj.has_character:  # Если в объекте есть персонаж
                obj.has_character = False  # Устанавливаем флаг отсутствия персонажа
                obj.remove_object(self.__character.get_crd())  # Удаляем персонажа из объекта
                break  # Выходим из цикла

    def __attack_enemy(self, place: Room | Corridor, crd: Coordinate) -> list[RogueEvent]:
        events, exp = place.get_object(crd).harm(*self.__character.attack())  # Атакуем врага
        if exp:  # Если враг повержен
            place.remove_object(crd)  # Удаляем врага из объекта
            events.extend(self.__character.add_experience(exp))  # Добавляем опыт персонажу
            RogueStats.get_instance().defeated_enemies += 1  # Увеличиваем количество поверженных врагов

        return events  # Возвращаем события

    def __pick_up_item(self, place: Room | Corridor, crd: Coordinate) -> list[RogueEvent]:
        events, picked_up = self.__character.pick_up_item(place.get_item(crd))  # Подбираем предмет
        if picked_up:  # Если предмет подобран
            place.remove_item(crd)  # Удаляем предмет из объекта

        return events  # Возвращаем события

    def __is_point_visible(self, crd: Coordinate) -> bool:
        """
        Оптимизированный алгоритм Брезенхема.
        Перебирает все клетки матрицы, через которые проходит луч между двумя точками.
        """
        y1, x1 = crd  # Координаты начальной точки
        y2, x2 = self.__character.get_crd()  # Координаты конечной точки
        dy, dx = abs(y2 - y1), abs(x2 - x1)  # Вычисляем разницу по y и x
        n = 1 + dx + dy  # Вычисляем количество шагов
        x_dir = 1 if x2 > x1 else -1  # Определяем направление по x
        y_dir = 1 if y2 > y1 else -1  # Определяем направление по y
        err = dx - dy  # Вычисляем начальную ошибку
        dx *= 2  # Удваиваем разницу по x
        dy *= 2  # Удваиваем разницу по y
        x = x1  # Устанавливаем начальную координату x
        y = y1  # Устанавливаем начальную координату y
        for _ in range(n):  # Проходим по всем шагам
            if not any(obj.is_in((y, x)) for obj in self.__rooms + self.__corridors):  # Если координата не находится в комнате или коридоре
                for corridor in self.__corridors:  # Проходим по коридорам
                    if crd in corridor.doors and crd == (y, x):  # Если координата находится в дверях коридора
                        break  # Выходим из цикла
                else:  # Если координата не находится в дверях коридора
                    return False  # Возвращаем False
            if err > 0:  # Если ошибка больше 0
                x += x_dir  # Увеличиваем координату x
                err -= dy  # Уменьшаем ошибку
            else:  # Если ошибка меньше или равна 0
                y += y_dir  # Увеличиваем координату y
                err += dx  # Увеличиваем ошибку

        return True  # Возвращаем True

    def make_rogue_move(self) -> tuple[list[RogueEvent], bool]:
        """
        Совершить ход всеми живыми объектами игры, кроме персонажа.
        """
        events = []  # Список для хранения событий
        alive = True  # Флаг наличия живых объектов
        cur_alive = True  # Флаг наличия живых объектов в текущем ходе

        for crd, obj, place in [
            (crd, obj, place_)
            for place_ in self.__rooms + self.__corridors
            for crd, obj in place_.objects.items()
            if not isinstance(obj, Character)
        ]:  # Проходим по всем объектам в комнатах и коридорах, кроме персонажа
            eff_events, able_to_move = obj.check_object_effects()  # Проверяем эффекты объектов
            events.extend(eff_events)  # Добавляем события в список
            if not able_to_move:  # Если объект не может двигаться
                continue  # Пропускаем объект
            if obj.is_engaged(crd) and (obj.status_engaged() or self.__is_point_visible(crd)):  # Если объект взаимодействует с персонажем
                domain_log.info(f"{obj.__class__.__name__} is chasing Character!")  # Логируем преследование персонажа
                obj.set_engaged_status()  # Устанавливаем статус взаимодействия
                g_events, cur_alive = self.__engaged_enemy_move(place, crd, obj)  # Двигаем объект
                events.extend(g_events)  # Добавляем события в список
            else:  # Если объект не взаимодействует с персонажем
                domain_log.info(f"{obj.__class__.__name__} makes a pattern move")  # Логируем случайное движение объекта
                self.__casual_enemy_move(place, crd, obj)  # Двигаем объект случайным образом

            alive = cur_alive if alive else alive  # Обновляем флаг наличия живых объектов

        return events, alive  # Возвращаем события и флаг наличия живых объектов

    def __casual_enemy_move(self, place: Room | Corridor, crd: Coordinate, enemy: Enemy):
        if enemy.pattern == MovePattern.STANDARD:  # Если паттерн движения стандартный
            self.__make_standard_move(place, crd, enemy)  # Двигаем объект стандартным образом
        elif enemy.pattern == MovePattern.DIAGONAL:  # Если паттерн движения диагональный
            self.__make_diagonal_move(place, crd, enemy)  # Двигаем объект диагональным образом
        elif enemy.pattern == MovePattern.JUMP:  # Если паттерн движения прыжок
            self.__make_jump_move(place, crd, enemy)  # Двигаем объект прыжком
        elif enemy.pattern == MovePattern.ITEM:  # Если паттерн движения предмет
            pass  # Пропускаем движение
        else:  # Если паттерн движения неизвестен
            domain_log.error("Unknown move pattern")  # Логируем ошибку
            raise NotImplementedError("Unknown move pattern")  # Выбрасываем исключение

    def __make_standard_move(self, place: Room | Corridor, crd: Coordinate, enemy: Enemy):
        if enemy.speed <= 0:  # Если скорость объекта равна 0
            domain_log.error("Enemy speed cannot be 0 for STANDARD move pattern")  # Логируем ошибку
            raise ValueError("Enemy speed cannot be 0 for STANDARD move pattern")  # Выбрасываем исключение

        y, x = crd  # Координаты объекта
        options = []  # Список для хранения возможных координат

        def is_in_place(crd: Coordinate):
            return any(obj.is_in_and_available_for_move(crd) for obj in self.__rooms + self.__corridors)  # Проверяем, находится ли координата в комнате или коридоре

        for x_ in range(x - enemy.speed, x):  # Проходим по координатам x
            if not is_in_place((y, x_)):  # Если координата не находится в комнате или коридоре
                break  # Выходим из цикла
            options.append((y, x_))  # Добавляем координату в список

        for x_ in range(x + 1, x + enemy.speed + 1):  # Проходим по координатам x
            if not is_in_place((y, x_)):  # Если координата не находится в комнате или коридоре
                break  # Выходим из цикла
            options.append((y, x_))  # Добавляем координату в список

        for y_ in range(y - enemy.speed, y):  # Проходим по координатам y
            if not is_in_place((y_, x)):  # Если координата не находится в комнате или коридоре
                break  # Выходим из цикла
            options.append((y_, x))  # Добавляем координату в список

        for y_ in range(y + 1, y + enemy.speed + 1):  # Проходим по координатам y
            if not is_in_place((y_, x)):  # Если координата не находится в комнате или коридоре
                break  # Выходим из цикла
            options.append((y_, x))  # Добавляем координату в список

        if not options:  # Если список возможных координат пуст
            return  # Выходим из метода
        self.__replace_enemy_on_map(place, crd, choice(options), enemy)  # Двигаем объект

    def __make_diagonal_move(self, place: Room | Corridor, crd: Coordinate, enemy: Enemy):
        if enemy.speed <= 0:  # Если скорость объекта равна 0
            domain_log.error("Enemy speed cannot be 0 for DIAGONAL move pattern")  # Логируем ошибку
            raise ValueError("Enemy speed cannot be 0 for DIAGONAL move pattern")  # Выбрасываем исключение

        if isinstance(place, Corridor):  # Если объект находится в коридоре
            self.__make_standard_move(place, crd, enemy)  # Двигаем объект стандартным образом
            return  # Выходим из метода

        y, x = crd  # Координаты объекта
        options = []  # Список для хранения возможных координат
        ur, ul, dr, dl = True, True, True, True  # Флаги для определения возможных направлений
        for i in range(1, enemy.speed + 1):  # Проходим по шагам
            if dr and any(obj.is_in_and_available_for_move((y + i, x + i)) for obj in self.__rooms + self.__corridors):  # Если координата находится в комнате или коридоре
                options.append((y + i, x + i))  # Добавляем координату в список
            else:  # Если координата не находится в комнате или коридоре
                dr = False  # Устанавливаем флаг невозможности движения в данном направлении
            if ur and any(obj.is_in_and_available_for_move((y - i, x + i)) for obj in self.__rooms + self.__corridors):  # Если координата находится в комнате или коридоре
                options.append((y - i, x + i))  # Добавляем координату в список
            else:  # Если координата не находится в комнате или коридоре
                ur = False  # Устанавливаем флаг невозможности движения в данном направлении
            if ul and any(obj.is_in_and_available_for_move((y - i, x - i)) for obj in self.__rooms + self.__corridors):  # Если координата находится в комнате или коридоре
                options.append((y - i, x - i))  # Добавляем координату в список
            else:  # Если координата не находится в комнате или коридоре
                ul = False  # Устанавливаем флаг невозможности движения в данном направлении
            if dl and any(obj.is_in_and_available_for_move((y + i, x - i)) for obj in self.__rooms + self.__corridors):  # Если координата находится в комнате или коридоре
                options.append((y + i, x - i))  # Добавляем координату в список
            else:  # Если координата не находится в комнате или коридоре
                dl = False  # Устанавливаем флаг невозможности движения в данном направлении

        if not options:  # Если список возможных координат пуст
            return  # Выходим из метода
        self.__replace_enemy_on_map(place, crd, choice(options), enemy)  # Двигаем объект

    def __engaged_enemy_move(
        self, place: Room | Corridor, crd: Coordinate, enemy: Enemy
    ) -> tuple[list[RogueEvent], bool]:
        events = []  # Список для хранения событий
        alive = True  # Флаг наличия живых объектов

        y, x = crd  # Координаты объекта
        possible_moves = [(y, x + 1), (y, x - 1), (y + 1, x), (y - 1, x)]  # Список возможных движений
        if self.__character.get_crd() in possible_moves:  # Если персонаж находится в зоне атаки
            sleep(0.2)  # Задержка
            domain_log.info(f"{enemy.__class__.__name__} attacks Character!")  # Логируем атаку персонажа
            g_events, alive = self.__character.harm(*enemy.attack())  # Атакуем персонажа
            events.extend(g_events)  # Добавляем события в список
        else:  # Если персонаж не находится в зоне атаки
            domain_log.info(f"{enemy.__class__.__name__} chases Character!")  # Логируем преследование персонажа
            c_y, c_x = self.__character.get_crd()  # Координаты персонажа
            min_dist = (abs(y - c_y) ** 2 + abs(x - c_x) ** 2) + 2  # Минимальное расстояние до персонажа
            best_move = []  # Список для хранения лучших движений
            for crd_ in possible_moves:  # Проходим по возможным движениям
                if not any(obj.is_in_and_available_for_move(crd_) for obj in self.__rooms + self.__corridors):  # Если координата не находится в комнате или коридоре
                    continue  # Пропускаем координату
                y_, x_ = crd_  # Координаты возможного движения
                if (new_dist := (abs(c_x - x_) ** 2 + abs(c_y - y_) ** 2)) <= min_dist:  # Если новое расстояние меньше или равно минимальному
                    if new_dist < min_dist and best_move:  # Если новое расстояние меньше минимального и список лучших движений не пуст
                        best_move.pop()  # Удаляем последнее движение из списка
                    min_dist = new_dist  # Устанавливаем новое минимальное расстояние
                    best_move.append((y_, x_))  # Добавляем координату в список лучших движений

            if best_move:  # Если список лучших движений не пуст
                self.__replace_enemy_on_map(place, crd, choice(best_move), enemy)  # Двигаем объект

        return events, alive  # Возвращаем события и флаг наличия живых объектов

    def __replace_enemy_on_map(self, place: Room | Corridor, crd: Coordinate, new_crd: Coordinate, enemy: Enemy):
        if place.is_in(new_crd):  # Если новая координата находится в комнате или коридоре
            place.add_object(new_crd, enemy)  # Добавляем объект в новую координату
            place.remove_object(crd)  # Удаляем объект из старой координаты
        else:  # Если новая координата не находится в комнате или коридоре
            for new_place in self.__rooms + self.__corridors:  # Проходим по комнатам и коридорам
                if new_place.is_in(new_crd):  # Если новая координата находится в комнате или коридоре
                    new_place.add_object(new_crd, enemy)  # Добавляем объект в новую координату
                    place.remove_object(crd)  # Удаляем объект из старой координаты
                    break  # Выходим из цикла

    @staticmethod
    def __make_jump_move(place: Room | Corridor, crd: Coordinate, enemy: Enemy) -> None:
        actual_move_crd = place.get_random_crd_in_zone(crd, enemy.speed)  # Получаем случайную координату в зоне
        place.add_object(actual_move_crd, enemy)  # Добавляем объект в новую координату
        place.remove_object(crd)  # Удаляем объект из старой координаты

    def is_exit(self) -> bool:
        crd = self.__character.get_crd()  # Координаты персонажа
        for room in self.__rooms:  # Проходим по комнатам
            if room.is_in(crd):  # Если координата находится в комнате
                return room.is_exit(crd)  # Возвращаем True, если координата является выходом

        return False  # Возвращаем False

    def get_cell(self, y: int, x: int) -> tuple[str, int]:
        """
        Вернуть символ карты для координаты.
        y = column, x = row
        """
        crd = (y, x)  # Координата клетки
        for corridor in self.__corridors:  # Проходим по коридорам
            if corridor.is_in_visible(crd):  # Если координата находится в коридоре
                if self.__is_character_near(crd) and self.__is_point_visible(crd):  # Если персонаж находится рядом и координата видима
                    self.__visited_corridors.add(crd)  # Добавляем координату в множество посещенных коридоров
                    return corridor.get_cell(y, x)  # Возвращаем символ и цвет клетки
                if crd in self.__visited_corridors:  # Если координата находится в множестве посещенных коридоров
                    return self.__map_symbol, self.__corridor_color  # Возвращаем символ и цвет клетки

        for room in self.__rooms:  # Проходим по комнатам
            if room.is_in(crd) and (
                room.has_character or (self.__is_character_near(crd) and self.__is_point_visible(crd))
            ):  # Если координата находится в комнате и персонаж находится в комнате или рядом и координата видима
                return room.get_cell(y, x)  # Возвращаем символ и цвет клетки
            if room.visited:  # Если комната посещена
                sym, col = room.get_border_symbol(y, x)  # Получаем символ и цвет границы комнаты
                if sym:  # Если символ существует
                    return sym, col  # Возвращаем символ и цвет клетки
                if room.is_exit(crd):  # Если координата является выходом
                    return room.get_cell(y, x)  # Возвращаем символ и цвет клетки

        return self.__map_symbol, self.__map_color  # Возвращаем символ и цвет пустой клетки

    def __is_character_near(self, crd: Coordinate) -> bool:
        visibility = 3  # Радиус видимости
        return (
            abs(crd[0] - self.__character.get_crd()[0]) <= visibility
            and abs(crd[1] - self.__character.get_crd()[1]) <= visibility
        )  # Возвращаем True, если персонаж находится рядом с координатой

    def drop_item(self, item: Item | None):
        if item:  # Если предмет существует
            y_c, x_c = self.__character.get_crd()  # Координаты персонажа
            y, x = y_c - 1, x_c - 1  # Начальные координаты для поиска места для предмета

            def place(y_, x_) -> bool:
                nonlocal item  # Используем nonlocal для изменения переменной item
                for obj in self.__rooms + self.__corridors:  # Проходим по комнатам и коридорам
                    if obj.is_in_and_available((y_, x_)):  # Если координата находится в комнате или коридоре и доступна
                        obj.add_item((y_, x_), item)  # Добавляем предмет в координату
                        return True  # Возвращаем True

                return False  # Возвращаем False

            step = 0  # Шаг для поиска места для предмета
            turn = 2  # Поворот для поиска места для предмета
            direction = 0  # Направление для поиска места для предмета
            while y <= self.height and x <= self.width:  # Пока координаты находятся в пределах карты
                if place(y, x):  # Если место для предмета найдено
                    domain_log.info(f"{item.__class__.__name__} dropped item.")  # Логируем выброс предмета
                    break  # Выходим из цикла

                if step == turn:  # Если шаг равен повороту
                    if direction == 3:  # Если направление равно 3
                        direction = 0  # Устанавливаем направление равным 0
                    else:  # Если направление не равно 3
                        direction += 1  # Увеличиваем направление
                        if direction == 3 or (direction == 1 and turn > 2):  # Если направление равно 3 или 1 и поворот больше 2
                            turn += 1  # Увеличиваем поворот
                    step = 0  # Устанавливаем шаг равным 0

                if direction == 0:  # Если направление равно 0
                    x += 1  # Увеличиваем координату x
                elif direction == 1:  # Если направление равно 1
                    y += 1  # Увеличиваем координату y
                elif direction == 2:  # Если направление равно 2
                    x -= 1  # Уменьшаем координату x
                elif direction == 3:  # Если направление равно 3
                    y -= 1  # Уменьшаем координату y
                step += 1  # Увеличиваем шаг
