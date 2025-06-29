import curses  # Импортируем библиотеку curses для работы с терминалом
from contextlib import suppress  # Импортируем функцию suppress из модуля contextlib для подавления исключений
from enum import Enum  # Импортируем класс Enum для создания перечислений
from time import sleep  # Импортируем функцию sleep из модуля time для задержек

from controller.game_info import GameInfo  # Импортируем класс GameInfo из модуля controller.game_info
from controller.sound.sound_controller import SoundController  # Импортируем класс SoundController из модуля controller.sound.sound_controller
from datalayer.stats import RogueStats  # Импортируем класс RogueStats из модуля datalayer.stats
from domain.map.level_map import LevelMap  # Импортируем класс LevelMap из модуля domain.map.level_map
from domain.map.settings import HEIGHT, MAX_LEVEL, WIDTH  # Импортируем константы HEIGHT, MAX_LEVEL, WIDTH из модуля domain.map.settings
from domain.objects.backpack.backpack import Backpack  # Импортируем класс Backpack из модуля domain.objects.backpack.backpack
from domain.objects.character import Character  # Импортируем класс Character из модуля domain.objects.character
from domain.objects.items.food import Food  # Импортируем класс Food из модуля domain.objects.items.food
from domain.objects.items.potion import Potion  # Импортируем класс Potion из модуля domain.objects.items.potion
from domain.objects.items.scroll import Scroll  # Импортируем класс Scroll из модуля domain.objects.items.scroll
from domain.objects.items.weapon import Weapon  # Импортируем класс Weapon из модуля domain.objects.items.weapon
from domain.objects.utils import RogueEvent  # Импортируем класс RogueEvent из модуля domain.objects.utils
from utils.logger import controller_log, domain_log  # Импортируем логгеры controller_log и domain_log из модуля utils.logger
from view.map_renderer import MapRenderer  # Импортируем класс MapRenderer из модуля view.map_renderer

class GameState(Enum):  # Определяем перечисление GameState для состояний игры
    INPUT = 1  # Состояние ожидания ввода
    INVENTORY = 2  # Состояние просмотра инвентаря
    DROP = 3  # Состояние выбора предмета для выброса
    DROP_SLOT = 4  # Состояние выброса предмета из слота
    ROGUE_MOVE = 5  # Состояние перемещения персонажа
    DEATH = 6  # Состояние смерти персонажа
    WIN = 7  # Состояние победы
    QUIT = 8  # Состояние выхода из игры
    END = 9  # Состояние завершения игры

class UserAction(Enum):  # Определяем перечисление UserAction для действий пользователя
    MOVE = "move"  # Действие перемещения
    QUIT = "q"  # Действие выхода
    ENTER = "\n"  # Действие подтверждения
    INVENTORY = "inventory"  # Действие открытия инвентаря
    SLOT = "slot"  # Действие выбора слота
    DROP = "l"  # Действие выброса предмета
    MUTE = "m"  # Действие отключения звука

    @classmethod
    def from_key(cls, key: str):  # Метод для преобразования клавиши в действие
        return (
            cls.MOVE
            if key in {"w", "a", "s", "d"}
            else cls.INVENTORY
            if key in {"h", "j", "k", "e"}
            else cls.SLOT
            if key.isdigit() and Backpack.MIN_SLOT <= int(key) <= Backpack.MAX_SLOT
            else cls(key)
        )

class Controller:  # Определяем класс Controller для управления игрой
    __ru_to_en = {  # Словарь для преобразования русских клавиш в английские
        "ц": "w",
        "ф": "a",
        "ы": "s",
        "в": "d",
        "р": "h",
        "о": "j",
        "л": "k",
        "у": "e",
        "й": "q",
        "д": "l",
        "ь": "m",
    }
    __inventory_mapping = {  # Словарь для сопоставления клавиш с типами предметов в инвентаре
        "j": Food,
        "k": Potion,
        "e": Scroll,
        "h": Weapon,
    }

    def __init__(self):  # Конструктор класса Controller
        self.rogue_stats = RogueStats()  # Инициализируем статистику игры
        self.stdscr = None  # Инициализируем экран
        self.renderer: MapRenderer | None = None  # Инициализируем рендерер карты
        self.game_info: GameInfo | None = None  # Инициализируем информацию об игре
        self.height = HEIGHT  # Устанавливаем высоту карты
        self.width = WIDTH  # Устанавливаем ширину карты
        self.sound_muted = False  # Устанавливаем флаг отключения звука
        self.state = GameState.INPUT  # Устанавливаем начальное состояние игры
        self.fsm = self.__generate_fsm()  # Генерируем конечный автомат состояний
        self.inventory_section = None  # Инициализируем секцию инвентаря
        self.level = 1  # Устанавливаем начальный уровень

        self.level_map = None  # Инициализируем карту уровня
        self.map = []  # Инициализируем карту
        self.__prev_hp = 0  # Инициализируем предыдущее значение здоровья персонажа

        SoundController()  # Инициализируем контроллер звука

        controller_log.info("{cls} initialized", cls=self.__class__.__name__)  # Логируем инициализацию контроллера

    def __generate_fsm(self):  # Метод для генерации конечного автомата состояний
        return {
            (GameState.INPUT, UserAction.MOVE): self.__move,  # Перемещение персонажа
            (GameState.INPUT, UserAction.QUIT): self.__quit_confirmation,  # Подтверждение выхода
            (GameState.INPUT, UserAction.ENTER): self.__enter,  # Вход в дверь или переход на следующий уровень
            (GameState.INPUT, UserAction.INVENTORY): self.__inventory,  # Открытие инвентаря
            (GameState.INPUT, UserAction.DROP): self.drop,  # Выброс предмета
            (GameState.INPUT, UserAction.MUTE): self.mute,  # Отключение звука
            (GameState.INVENTORY, UserAction.QUIT): self.close_inventory,  # Закрытие инвентаря
            (GameState.INVENTORY, UserAction.SLOT): self.slot,  # Выбор слота в инвентаре
            (GameState.DROP, UserAction.INVENTORY): self.__inventory,  # Открытие инвентаря для выброса предмета
            (GameState.DROP, UserAction.QUIT): self.close_inventory,  # Закрытие инвентаря при выбросе предмета
            (GameState.DROP_SLOT, UserAction.SLOT): self.drop_slot,  # Выброс предмета из слота
            (GameState.DROP_SLOT, UserAction.QUIT): self.close_inventory,  # Закрытие инвентаря при выбросе предмета из слота
            (GameState.DEATH, UserAction.QUIT): self.__quit_confirmation,  # Подтверждение выхода при смерти персонажа
            (GameState.QUIT, UserAction.ENTER): self.__quit,  # Выход из игры
            (GameState.QUIT, UserAction.QUIT): self.__cancel_quit,  # Отмена выхода из игры
        }

    def __curser_init(self, stdscr):  # Метод для инициализации экрана
        self.stdscr = stdscr  # Устанавливаем экран
        self.stdscr.clear()  # Очищаем экран
        curses.cbreak()  # Включаем режим cbreak для мгновенного получения ввода
        self.stdscr.keypad(True)  # Включаем режим keypad для обработки специальных клавиш

    def start_rogue(self):  # Метод для запуска игры
        """
        Запустить контроллер Rogue Game.
        """
        curses.wrapper(self.__start)  # Запускаем игру в оболочке curses

    def __start(self, stdscr):  # Метод для начала игры
        self.__curser_init(stdscr)  # Инициализируем экран

        while self.state != GameState.END:  # Основной цикл игры
            SoundController.get_instance().intro.play(-1)  # Включаем музыку вступления
            self.renderer = MapRenderer(self.height, self.width)  # Инициализируем рендерер карты
            self.renderer.show_intro(pause=True)  # Показываем вступление
            self.__try_load()  # Пытаемся загрузить сохраненную игру
            if self.state == GameState.END:  # Если игра завершена, выходим из цикла
                break
            self.renderer.clear_intro()  # Очищаем вступление
            self.game_info = GameInfo(self.level)  # Инициализируем информацию об игре
            self.level_map = LevelMap(self.height, self.width, self.level, 1)  # Инициализируем карту уровня
            SoundController.get_instance().intro.stop()  # Останавливаем музыку вступления
            self.__game_loop()  # Запускаем игровой цикл

            self.rogue_stats.rogue_level = self.level  # Обновляем уровень персонажа

            if self.state != GameState.END:  # Если игра не завершена, показываем статистику
                self.renderer.show_stats(self.rogue_stats.get_sorted_stats())
                self.state = GameState.INPUT  # Устанавливаем состояние ожидания ввода
                self.rogue_stats = RogueStats()  # Сбрасываем статистику
                SoundController.get_instance().game_over.stop()  # Останавливаем музыку игры
                self.renderer.show_intro(pause=True)  # Показываем вступление

    def __try_load(self):  # Метод для загрузки сохраненной игры
        """
        Найти сохраненную игру, создать персонажа.
        """
        if save := self.rogue_stats.load_json_save():  # Если есть сохраненная игра, загружаем её
            user_input = UserAction.DROP
            with suppress(ValueError):  # Подавляем исключения при некорректном вводе
                user_input = UserAction.from_key(self.__normalize_input(self.renderer.render_load_question()))

            if user_input == UserAction.ENTER:  # Если пользователь подтверждает загрузку
                self.__load_save(save)
                return
            if user_input == UserAction.QUIT:  # Если пользователь отменяет загрузку
                self.state = GameState.END
                return

        self.__start_new_game()  # Начинаем новую игру

    def __load_save(self, save):  # Метод для загрузки сохраненной игры
        if Character.get_instance():  # Если персонаж уже существует, сбрасываем его
            Character.reset_instance()
        Character(save["nickname"])._load(**save["character_state"])  # Загружаем состояние персонажа
        self.level = save["stats"]["rogue_level"]  # Устанавливаем уровень персонажа
        self.rogue_stats._load(**save["stats"])  # Загружаем статистику
        self.__prev_hp = Character.get_instance().hp  # Обновляем предыдущее значение здоровья персонажа

    def __start_new_game(self):  # Метод для начала новой игры
        user_input = UserAction.DROP
        with suppress(ValueError):  # Подавляем исключения при некорректном вводе
            if UserAction.from_key(self.__normalize_input(self.renderer.render_start_question())) == UserAction.QUIT:
                self.state = GameState.END
                return

        ch_name = self.renderer.get_player_name()  # Получаем имя персонажа
        while not self.rogue_stats.check_nickname(ch_name):  # Проверяем корректность имени
            with suppress(ValueError):
                user_input = UserAction.from_key(self.renderer.confirm_name())

            if user_input == UserAction.ENTER:  # Если пользователь подтверждает имя
                break

            self.renderer.show_intro()  # Показываем вступление
            ch_name = self.renderer.get_player_name()  # Получаем имя персонажа

        if Character.get_instance():  # Если персонаж уже существует, сбрасываем его
            Character.reset_instance()
            self.level = 1  # Устанавливаем начальный уровень
        Character(ch_name)  # Создаем нового персонажа
        self.__prev_hp = Character.get_instance().hp  # Обновляем предыдущее значение здоровья персонажа

    def __game_loop(self):  # Метод для игрового цикла
        self.renderer.render_game_info(self.game_info)  # Рендерим информацию об игре
        self.__draw_map()  # Рисуем карту
        self.renderer.draw_event_box()  # Рисуем окно событий
        self.renderer.render_controls()  # Рендерим управление
        SoundController.get_instance().play_background()  # Включаем фоновую музыку

        controller_log.debug("loop started")  # Логируем начало цикла
        while self.state not in {GameState.DEATH, GameState.WIN, GameState.END}:  # Основной цикл игры
            if self.state in {GameState.INVENTORY, GameState.DROP_SLOT}:  # Если состояние инвентаря или выброса предмета
                self.__input_to_action(
                    self.renderer.draw_inventory(
                        self.inventory_content,
                        f"Введите номер предмета, который хотите "
                        f"{'выбрать' if self.state == GameState.INVENTORY else 'выбросить'}",
                        "1 - 9",
                    )
                )
            elif self.state == GameState.QUIT:  # Если состояние выхода
                SoundController.get_instance().mute(0)  # Отключаем звук
                self.__input_to_action(self.renderer.draw_exit_window())
                SoundController.get_instance().mute(not self.sound_muted)  # Включаем звук
            else:  # В других состояниях
                self.__input_to_action(self.renderer.get_input(self.state.value))

            if self.state == GameState.ROGUE_MOVE:  # Если состояние перемещения персонажа
                self.state = GameState.INPUT  # Устанавливаем состояние ожидания ввода
                self.__update_rogue_state()  # Обновляем состояние персонажа

        SoundController.get_instance().stop_background()  # Останавливаем фоновую музыку
        if self.state == GameState.DEATH:  # Если персонаж погиб
            self.rogue_stats.dump_json_stats()  # Сохраняем статистику
            SoundController.get_instance().game_over.play(-1)  # Включаем музыку игры
            self.rogue_stats.remove_save()  # Удаляем сохранение
            self.renderer.show_intro(start=False, death=True)  # Показываем вступление с информацией о смерти
        if self.state == GameState.WIN:  # Если персонаж победил
            self.rogue_stats.dump_json_stats()  # Сохраняем статистику
            SoundController.get_instance().win.play()  # Включаем музыку победы
            self.rogue_stats.remove_save()  # Удаляем сохранение
            self.renderer.show_intro(start=False, death=False)  # Показываем вступление с информацией о победе

    def __input_to_action(self, key: int):  # Метод для обработки ввода пользователя
        controller_log.info(f"Processing user input: {key}")  # Логируем ввод пользователя

        key_str = self.__normalize_input(key)  # Нормализуем ввод

        controller_log.info(f"Normalized user input to: {key_str}")  # Логируем нормализованный ввод

        user_input = None
        try:
            user_input = UserAction.from_key(key_str)  # Преобразуем ввод в действие
            controller_log.info(f"{self.state} {user_input}")
        except ValueError:
            controller_log.info(f"key: '{key_str}' is not in input options. Ignore")  # Логируем ошибку ввода

        if user_input and (action := self.fsm.get((self.state, user_input))):  # Если действие найдено
            controller_log.info(f"action: {action.__name__}")  # Логируем действие
            events = action(key_str)  # Выполняем действие
            for event in events:  # Обрабатываем события
                self.renderer.render_event(event)

            self.game_info.refresh(self.level)  # Обновляем информацию об игре
            self.renderer.render_game_info(self.game_info)  # Рендерим информацию об игре
            self.__draw_map()  # Рисуем карту

    def __update_rogue_state(self):  # Метод для обновления состояния персонажа
        controller_log.info("Getting Rogue update")  # Логируем обновление состояния
        events, alive = self.level_map.make_rogue_move()  # Получаем события и состояние персонажа

        for event in events:  # Обрабатываем события
            self.renderer.render_event(event)

        if not alive:  # Если персонаж погиб
            controller_log.info("state = death")  # Логируем состояние смерти
            self.state = GameState.DEATH  # Устанавливаем состояние смерти
            self.renderer.render_event(RogueEvent("Персонаж погиб"))  # Рендерим событие смерти

        self.game_info.refresh(self.level)  # Обновляем информацию об игре
        self.__draw_map()  # Рисуем карту
        self.renderer.render_game_info(self.game_info)  # Рендерим информацию об игре

    def __normalize_input(self, key: str | int) -> str:  # Метод для нормализации ввода
        key_str = chr(key).lower() if isinstance(key, int) else key.lower()  # Преобразуем ввод в строку и приводим к нижнему регистру

        return self.__ru_to_en.get(key_str, key_str)  # Возвращаем нормализованный ввод

    def __move(self, direction: str) -> list[RogueEvent]:  # Метод для перемещения персонажа
        self.state = GameState.ROGUE_MOVE  # Устанавливаем состояние перемещения
        return self.level_map.move_character(direction)  # Возвращаем события перемещения

    def __quit_confirmation(self, _=None):  # Метод для подтверждения выхода
        controller_log.debug("__quit_confirmation")  # Логируем подтверждение выхода
        self.state = GameState.QUIT  # Устанавливаем состояние выхода
        return []

    def __quit(self, _=None) -> list[RogueEvent]:  # Метод для выхода из игры
        self.state = GameState.END  # Устанавливаем состояние завершения игры
        return []

    def __cancel_quit(self, _=None):  # Метод для отмены выхода
        self.state = GameState.INPUT  # Устанавливаем состояние ожидания ввода
        return []

    def __calc_complexity_coef(self) -> float:  # Метод для расчета коэффициента сложности
        max_coef = 2
        coef = Character.get_instance().hp / (self.__prev_hp / 2)  # Рассчитываем коэффициент сложности
        self.__prev_hp = Character.get_instance().hp  # Обновляем предыдущее значение здоровья персонажа
        controller_log.info(f"Complexity coef: {coef}")  # Логируем коэффициент сложности
        return coef if coef <= max_coef else max_coef  # Возвращаем коэффициент сложности

    def __enter(self, _=None) -> list[RogueEvent]:  # Метод для входа в дверь или перехода на следующий уровень
        controller_log.info("Enter")  # Логируем вход
        events = []

        if self.level_map.is_exit() and self.level == MAX_LEVEL:  # Если персонаж на последнем уровне и находится у выхода
            self.state = GameState.WIN  # Устанавливаем состояние победы
            events = [RogueEvent("Поздравляем, вы прошли игру!")]  # Добавляем событие победы
            sleep(1)  # Задержка
        elif self.level_map.is_exit():  # Если персонаж находится у выхода
            controller_log.info("On exit")  # Логируем вход в выход
            self.level += 1  # Увеличиваем уровень
            self.rogue_stats.rogue_level += 1  # Обновляем уровень персонажа
            self.level_map = LevelMap(self.height, self.width, self.level, self.__calc_complexity_coef())  # Инициализируем карту уровня
            self.rogue_stats.dump_json_save()  # Сохраняем статистику
            events = [RogueEvent(f"Вы перешли на уровень {self.level}")]  # Добавляем событие перехода на следующий уровень

        return events

    def __inventory(self, key: str) -> list[RogueEvent]:  # Метод для открытия инвентаря
        self.inventory_section = self.__inventory_mapping[key]  # Устанавливаем секцию инвентаря
        self.state = GameState(self.state.value + 1)  # Устанавливаем состояние инвентаря
        controller_log.info(f"opening {self.inventory_section, __name__} section")  # Логируем открытие секции инвентаря
        self.inventory_content = Backpack.get_instance().show_items(self.inventory_section)  # Получаем содержимое инвентаря

        events = []
        if len(self.inventory_content) == 0:  # Если инвентарь пуст
            events.append(RogueEvent(f"У вас нет предметов типа {self.inventory_section(1).type}"))  # Добавляем событие отсутствия предметов
            self.state = GameState(self.state.value - 1)  # Устанавливаем предыдущее состояние

        return events

    def close_inventory(self, _=None) -> list[RogueEvent]:  # Метод для закрытия инвентаря
        self.state = GameState.INPUT  # Устанавливаем состояние ожидания ввода
        return []

    def slot(self, key: str) -> list[RogueEvent]:  # Метод для выбора слота в инвентаре
        slot = int(key)  # Преобразуем ввод в номер слота
        controller_log.info(f"using slot {slot} in {self.inventory_section, __name__} section")  # Логируем выбор слота
        events, item = Backpack.get_instance().use_item(self.inventory_section, slot)  # Используем предмет из слота
        if item:  # Если предмет найден
            domain_log.debug(f"item: {item.__str__()}")  # Логируем предмет
            if isinstance(item, Weapon):  # Если предмет - оружие
                w_events, weapon_to_drop = Character.get_instance().equip_weapon(item)  # Экипируем оружие
                events.extend(w_events)  # Добавляем события экипировки
                Backpack.get_instance().drop_weapon(weapon_to_drop)  # Выбрасываем старое оружие
                self.level_map.drop_item(weapon_to_drop)  # Выбрасываем старое оружие на карту
            else:  # Если предмет - не оружие
                events.extend(Character.get_instance().use_item(item))  # Используем предмет
            self.state = GameState.ROGUE_MOVE  # Устанавливаем состояние перемещения
        return events

    def drop(self, _=None) -> list[RogueEvent]:  # Метод для выброса предмета
        self.state = GameState.DROP  # Устанавливаем состояние выброса предмета
        return []

    def drop_slot(self, key: str) -> list[RogueEvent]:  # Метод для выброса предмета из слота
        slot = int(key)  # Преобразуем ввод в номер слота
        events, item = Backpack.get_instance().drop_item(self.inventory_section, slot)  # Выбрасываем предмет из слота
        if item:  # Если предмет найден
            if isinstance(item, Weapon) and item is Character.get_instance().held_weapon:  # Если предмет - оружие и экипировано
                events, _ = Character.get_instance().drop_weapon()  # Выбрасываем оружие
            self.level_map.drop_item(item)  # Выбрасываем предмет на карту
            self.state = GameState.ROGUE_MOVE  # Устанавливаем состояние перемещения

        return events

    def mute(self, _=None) -> list[RogueEvent]:  # Метод для отключения звука
        events = [RogueEvent("Звук включен" if self.sound_muted else "Звук выключен")]  # Добавляем событие отключения звука

        SoundController.get_instance().mute(float(self.sound_muted))  # Отключаем звук
        self.sound_muted = not self.sound_muted  # Инвертируем флаг отключения звука

        return events

    def __draw_map(self):  # Метод для рисования карты
        self.renderer.clear_game_window()  # Очищаем окно игры
        for y in range(1, self.height - 3):  # Проходим по высоте карты
            for x in range(1, self.width + 1):  # Проходим по ширине карты
                self.renderer.render_map_crd(y - 1, x - 1, *self.level_map.get_cell(y - 1, x - 1))  # Рисуем ячейку карты
        self.renderer.refresh_game_window()  # Обновляем окно игры
