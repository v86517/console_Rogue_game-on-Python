import random

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage
from datalayer.stats import RogueStats
from domain.objects.items.item import Item
from domain.objects.utils import IS_MACOS, Effects, RogueEffect, RogueEvent

food_types = {"яблоко": 10, "манго": 30, "хлеб": 50, "кроличье рагу": 100, "эликсир жизни": 1000}  # полный хилл

base_weights = {"яблоко": 70, "манго": 15, "хлеб": 1, "кроличье рагу": 4.9, "эликсир жизни": 0.1}

alpha_values = {"яблоко": 0.25, "манго": 0.25, "хлеб": 0.2, "кроличье рагу": 0.3, "эликсир жизни": 0.001}

messages = ["Неплохо подкрепились", "Это было не очень вкусно...", "Фу какая гадость"]


class Food(Item):
    type = "еда"
    symbol = "\u2668" if IS_MACOS else "f"
    color = 9

    def __init__(self, level: int):
        self.name, self.health_points = self._random_type(level, food_types, base_weights, alpha_values)
        self.__effect = RogueEffect(Effects.HEALTH, self.health_points, 0)
        self.__add_sound = SoundController.get_instance().get_sound(SoundType.Food, SoundUsage.add)
        self.__use_sound = SoundController.get_instance().get_sound(SoundType.Food, SoundUsage.use)

    @property
    def effect(self):
        return self.__effect

    def pick_up(self) -> list[RogueEvent]:
        self.__add_sound.play()
        return [RogueEvent(f"Вы нашли {self.name}", 11)]

    def drop(self) -> tuple[list[RogueEvent], Item]:
        return [RogueEvent(f"Вы выбросили {self.name}")], self

    def use(self) -> tuple[list[RogueEvent], RogueEffect]:
        self.__use_sound.play()
        RogueStats.get_instance().eaten_food += 1
        return [RogueEvent(random.choice(messages))], self.__effect  # рандомное сообщение еда вкусная или нет

    def _dump(self) -> dict:
        return {"name": self.name, "hp": self.health_points}

    def _load(self, name: str, hp: int):
        self.name = name
        self.health_points = hp
        self.__effect = RogueEffect(Effects.HEALTH, self.health_points, 0)

    def __str__(self):
        return f"{self.name}"

    def __lt__(self, other: Item):
        return self.health_points < other.health_points
