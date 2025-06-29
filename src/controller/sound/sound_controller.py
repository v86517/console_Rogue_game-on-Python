from enum import Enum  # Импортируем класс Enum для создания перечислений

import pygame  # Импортируем библиотеку pygame для работы с аудио

from utils.utils import get_project_root  # Импортируем функцию get_project_root из модуля utils.utils

class SoundType(Enum):  # Определяем перечисление SoundType для типов звуков
    Food = 1  # Звук для еды
    Gold = 2  # Звук для золота
    Weapon = 3  # Звук для оружия
    Enemy = 4  # Звук для врага
    Ghost = 5  # Звук для призрака
    Mimic = 6  # Звук для мимика
    Ogre = 7  # Звук для огра
    SnakeMage = 8  # Звук для змеиного мага
    Vampire = 9  # Звук для вампира
    Zombie = 10  # Звук для зомби
    Character = 11  # Звук для персонажа
    Door = 12  # Звук для двери
    Key = 13  # Звук для ключа
    Potion = 14  # Звук для зелья
    Scroll = 15  # Звук для свитка
    Level = 16  # Звук для уровня

class SoundUsage(Enum):  # Определяем перечисление SoundUsage для типов использования звуков
    add = 1  # Звук добавления
    use = 2  # Звук использования
    hit = 3  # Звук попадания
    miss = 4  # Звук промаха
    engaged = 5  # Звук взаимодействия
    closed = 6  # Звук закрытия
    open = 7  # Звук открытия

class SoundController:  # Определяем класс SoundController для управления звуками
    __instance = None  # Приватное статическое поле для хранения единственного экземпляра класса

    def __new__(cls, *args, **kwargs):  # Переопределяем метод __new__ для реализации паттерна Singleton
        if cls.__instance is None:  # Если экземпляр не создан
            cls.__instance = super().__new__(cls, *args, **kwargs)  # Создаем новый экземпляр
        return cls.__instance  # Возвращаем единственный экземпляр

    @classmethod
    def get_instance(cls):  # Метод для получения единственного экземпляра класса
        return cls.__instance  # Возвращаем единственный экземпляр

    def __init__(self):  # Конструктор класса SoundController
        pygame.init()  # Инициализируем pygame
        #pygame.mixer.init()  # Инициализируем микшер pygame (закомментировано)
        self.sounds = {  # Словарь для хранения звуков
            (SoundType.Food, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/add_food.ogg"),  # Звук добавления еды
            (SoundType.Food, SoundUsage.use): pygame.mixer.Sound(get_project_root() / "misc/sounds/use_food.ogg"),  # Звук использования еды
            (SoundType.Gold, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/add_gold.ogg"),  # Звук добавления золота
            (SoundType.Weapon, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/add_weapon.ogg"),  # Звук добавления оружия
            (SoundType.Weapon, SoundUsage.use): pygame.mixer.Sound(get_project_root() / "misc/sounds/use_weapon.ogg"),  # Звук использования оружия
            (SoundType.Enemy, SoundUsage.miss): pygame.mixer.Sound(get_project_root() / "misc/sounds/miss_sound.ogg"),  # Звук промаха врага
            (SoundType.Enemy, SoundUsage.engaged): pygame.mixer.Sound(get_project_root() / "misc/sounds/enemy_engaged.ogg"),  # Звук взаимодействия с врагом
            (SoundType.Ghost, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_ghost.ogg"),  # Звук попадания по призраку
            (SoundType.Mimic, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_ogre.ogg"),  # Звук попадания по мимику
            (SoundType.Ogre, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_ogre.ogg"),  # Звук попадания по огру
            (SoundType.SnakeMage, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_snake_mage.ogg"),  # Звук попадания по змеиному магу
            (SoundType.Vampire, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_vampire.ogg"),  # Звук попадания по вампиру
            (SoundType.Zombie, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_zombie.ogg"),  # Звук попадания по зомби
            (SoundType.Character, SoundUsage.hit): pygame.mixer.Sound(get_project_root() / "misc/sounds/hit_character.ogg"),  # Звук попадания по персонажу
            (SoundType.Character, SoundUsage.miss): pygame.mixer.Sound(get_project_root() / "misc/sounds/miss_sound.ogg"),  # Звук промаха персонажа
            (SoundType.Door, SoundUsage.closed): pygame.mixer.Sound(get_project_root() / "misc/sounds/door_closed.ogg"),  # Звук закрытия двери
            (SoundType.Door, SoundUsage.open): pygame.mixer.Sound(get_project_root() / "misc/sounds/door_open.ogg"),  # Звук открытия двери
            (SoundType.Key, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/key_add.ogg"),  # Звук добавления ключа
            (SoundType.Potion, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/potion_add.ogg"),  # Звук добавления зелья
            (SoundType.Potion, SoundUsage.use): pygame.mixer.Sound(get_project_root() / "misc/sounds/potion_use.ogg"),  # Звук использования зелья
            (SoundType.Scroll, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/scroll_add.ogg"),  # Звук добавления свитка
            (SoundType.Scroll, SoundUsage.use): pygame.mixer.Sound(get_project_root() / "misc/sounds/scroll_use.ogg"),  # Звук использования свитка
            (SoundType.Level, SoundUsage.open): pygame.mixer.Sound(get_project_root() / "misc/sounds/next_level.ogg"),  # Звук перехода на следующий уровень
            (SoundType.Character, SoundUsage.add): pygame.mixer.Sound(get_project_root() / "misc/sounds/level_up.ogg"),  # Звук повышения уровня персонажа
        }
        pygame.mixer.music.load(get_project_root() / "misc/sounds/background.ogg")  # Загружаем фоновую музыку
        self.intro = pygame.mixer.Sound(get_project_root() / "misc/sounds/intro_2.ogg")  # Звук вступления
        self.game_over = pygame.mixer.Sound(get_project_root() / "misc/sounds/gameover.ogg")  # Звук конца игры
        self.win = pygame.mixer.Sound(get_project_root() / "misc/sounds/win.ogg")  # Звук победы

        self.mute(1)  # Отключаем звук

    @staticmethod
    def play_background() -> None:  # Метод для воспроизведения фоновой музыки
        pygame.mixer.music.play(loops=-1)  # Воспроизводим фоновую музыку в бесконечном цикле

    @staticmethod
    def stop_background() -> None:  # Метод для остановки фоновой музыки
        pygame.mixer.music.stop()  # Останавливаем воспроизведение фоновой музыки

    def get_sound(self, object_type, usage):  # Метод для получения звука по типу и использованию
        return self.sounds.get((object_type, usage))  # Возвращаем звук из словаря

    def mute(self, volume):  # Метод для отключения звука
        pygame.mixer.music.set_volume(volume * 0.4)  # Устанавливаем громкость фоновой музыки
        for sound in self.sounds.values():  # Для всех звуков в словаре
            sound.set_volume(volume * 0.7)  # Устанавливаем громкость звука
        self.game_over.set_volume(volume * 0.7)  # Устанавливаем громкость звука конца игры
        self.win.set_volume(volume * 0.7)  # Устанавливаем громкость звука победы
        self.intro.set_volume(volume * 0.7)  # Устанавливаем громкость звука вступления
