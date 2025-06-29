from domain.objects.character import Character  # Импортируем класс Character из модуля domain.objects.character
from utils.logger import controller_log  # Импортируем логгер controller_log из модуля utils.logger

class GameInfo:  # Определяем класс GameInfo для хранения информации об игре
    def __init__(self, floor):  # Конструктор класса GameInfo
        self.floor = 0  # Инициализируем текущий этаж
        self.hp = 0  # Инициализируем текущее здоровье персонажа
        self.max_hp = 0  # Инициализируем максимальное здоровье персонажа
        self.str = 0  # Инициализируем силу персонажа
        self.ag = 0  # Инициализируем ловкость персонажа
        self.gold = 0  # Инициализируем количество золота персонажа
        self.exp = 0  # Инициализируем количество опыта персонажа
        self.max_exp = 0  # Инициализируем максимальное количество опыта персонажа
        self.level = 0  # Инициализируем уровень персонажа

        self.character = Character.get_instance()  # Получаем экземпляр персонажа
        self.refresh(floor)  # Обновляем информацию об игре

        controller_log.info("{cls} initialized.", cls=self.__class__.__name__)  # Логируем инициализацию класса GameInfo

    def refresh(self, floor):  # Метод для обновления информации об игре
        if self.character is None:  # Если персонаж не существует
            controller_log.error("Initializing GameInfo when Character doesn't exist")  # Логируем ошибку
            raise ValueError("Character doesn't exist")  # Выбрасываем исключение

        self.floor = floor  # Обновляем текущий этаж
        self.hp = self.character.hp  # Обновляем текущее здоровье персонажа
        self.max_hp = self.character.max_hp  # Обновляем максимальное здоровье персонажа
        self.str = self.character.strength  # Обновляем силу персонажа
        self.ag = self.character.agility  # Обновляем ловкость персонажа
        self.gold = self.character.gold  # Обновляем количество золота персонажа
        self.exp = self.character.exp  # Обновляем количество опыта персонажа
        self.max_exp = self.character.max_exp  # Обновляем максимальное количество опыта персонажа
        self.level = self.character.level  # Обновляем уровень персонажа
