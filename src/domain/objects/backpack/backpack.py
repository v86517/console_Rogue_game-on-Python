from domain.objects.backpack.item_list import ItemList
from domain.objects.items import Weapon
from domain.objects.items.gold import Gold
from domain.objects.items.item import Item
from domain.objects.utils import RogueEvent
from utils.logger import domain_log

class Backpack:
    __instance = None  # Приватное статическое поле для хранения единственного экземпляра класса

    MIN_SLOT = 1  # Минимальный слот для предметов
    MAX_SLOT = 9  # Максимальный слот для предметов

    def __new__(cls, *args, **kwargs):  # Переопределяем метод __new__ для реализации паттерна Singleton
        if cls.__instance is None:  # Если экземпляр не создан
            cls.__instance = super().__new__(cls, *args, **kwargs)  # Создаем новый экземпляр
        return cls.__instance  # Возвращаем единственный экземпляр

    @classmethod
    def get_instance(cls):  # Метод для получения единственного экземпляра класса
        return cls.__instance  # Возвращаем единственный экземпляр

    def __init__(self):  # Конструктор класса Backpack
        self.items = ItemList()  # Инициализируем список предметов

    def add_item(self, item: Item) -> tuple[list[RogueEvent], bool]:  # Метод для добавления предмета в рюкзак
        domain_log.info("Adding item to backpack")  # Логируем добавление предмета
        if isinstance(item, Gold):  # Если предмет - золото
            domain_log.error("Adding gold to backpack")  # Логируем ошибку
            raise TypeError("You're trying to add gold to backpack")  # Выбрасываем исключение
        events = []  # Список для хранения событий

        added = self.items.add(item)  # Добавляем предмет в список
        if added:  # Если предмет добавлен
            events.extend(item.pick_up())  # Добавляем события подбора предмета
        else:  # Если предмет не добавлен
            events.append(RogueEvent(f"Отделение для предметов типа '{item.type}' переполнено."))  # Добавляем событие о переполнении

        return events, added  # Возвращаем события и флаг добавления

    def drop_item(self, item_type, slot: int) -> tuple[list[RogueEvent], Item | None]:  # Метод для выброса предмета из рюкзака
        domain_log.info(f"Dropping item from slot {slot} {item_type.__name__} compartment of backpack")  # Логируем выброс предмета
        type_size = self.items.type_size(item_type)  # Получаем размер отделения для предметов
        item = None  # Инициализируем переменную для хранения предмета
        events = []  # Список для хранения событий

        if 0 < slot <= type_size:  # Если слот в пределах размера отделения
            events, item = self.items.drop(item_type, slot - 1)  # Выбрасываем предмет из слота
        else:  # Если слот вне пределов размера отделения
            events.append(
                RogueEvent(
                    "Отделение пусто" if type_size == 0 else f"Неверный слот. Введите слот от 1) до {type_size})"
                )
            )  # Добавляем событие о неверном слоте

        return events, item  # Возвращаем события и предмет

    def drop_weapon(self, weapon: Weapon | None):  # Метод для выброса оружия из рюкзака
        if weapon:  # Если оружие существует
            self.items.weapons.remove(weapon)  # Удаляем оружие из списка

    def use_item(self, item_type, slot: int) -> tuple[list[RogueEvent], Item | None]:  # Метод для использования предмета из рюкзака
        domain_log.info(f"Using item from slot {slot} {item_type.__name__} section of backpack")  # Логируем использование предмета
        events = []  # Список для хранения событий
        item = None  # Инициализируем переменную для хранения предмета

        type_size = self.items.type_size(item_type)  # Получаем размер отделения для предметов
        if 0 < slot <= type_size:  # Если слот в пределах размера отделения
            use_events, item = self.items.use(item_type, slot - 1)  # Используем предмет из слота
            events.extend(use_events)  # Добавляем события использования предмета
        else:  # Если слот вне пределов размера отделения
            events.append(
                RogueEvent(
                    "Отделение пусто" if type_size == 0 else f"Неверный слот. Введите слот от 1) до {type_size})"
                )
            )  # Добавляем событие о неверном слоте

        return events, item  # Возвращаем события и предмет

    def show_items(self, item_type):  # Метод для отображения содержимого рюкзака
        domain_log.info(f"Get backpack contents for {item_type.__name__}")  # Логируем отображение содержимого
        return self.items.show(item_type)  # Возвращаем содержимое отделения для предметов
