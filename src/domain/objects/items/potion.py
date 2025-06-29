import random
from enum import Enum

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage
from datalayer.stats import RogueStats
from domain.objects.items.item import Item
from domain.objects.utils import IS_MACOS, Effects, RogueEffect, RogueEvent


class Power(Enum):
    FIRST = (10, "I")
    SECOND = (20, "II")
    THIRD = (30, "III")

    def __init__(self, time, label):
        self._time = time
        self._label = label

    @property
    def time(self):
        return self._time

    @property
    def label(self):
        return self._label


class Potion(Item):
    symbol = "&" if IS_MACOS else "𐃰"
    type = "зелье"
    color = 25

    def __init__(self, level: int):
        self.feature = random.choice(list(Effects)[:3])
        feature_base = 100 if self.feature == Effects.MAX_HEALTH else 15 if self.feature == Effects.STRENGTH else 12
        self.points = round(
            random.uniform(feature_base * 0.3 * pow(1.15, level - 1), feature_base * 0.5 * pow(1.15, level - 1))
        )

        power = random.choice(list(Power))
        self.power = power.label
        self.time = power.time  # секунды
        self.__effect = RogueEffect(self.feature, self.points, self.time)
        self.__add_sound = SoundController.get_instance().get_sound(SoundType.Potion, SoundUsage.add)
        self.__use_sound = SoundController.get_instance().get_sound(SoundType.Potion, SoundUsage.use)

    @property
    def effect(self):
        return self.__effect

    def pick_up(self) -> list[RogueEvent]:
        self.__add_sound.play()
        return [RogueEvent(f"Вы нашли зелье {self.feature.label} {self.power}", 11)]

    def drop(self) -> tuple[list[RogueEvent], Item]:
        return [RogueEvent(f"Вы выбросили зелье {self.feature.label} {self.power}")], self

    def use(self) -> tuple[list[RogueEvent], RogueEffect]:
        self.__use_sound.play()
        RogueStats.get_instance().used_potions += 1
        if self.feature == Effects.AGILITY:
            spelling = "Ваша ловкость временно возрасла"
        elif self.feature == Effects.STRENGTH:
            spelling = "Ваша сила временно возрасла"
        else:
            spelling = "Ваш запас здоровья временно увеличился"

        return [RogueEvent(spelling)], self.__effect

    def _dump(self) -> dict:
        return {
            "feature": self.feature.name,
            "points": self.points,
            "power": self.power,
            "time": self.time,
        }

    def _load(self, feature: str, points: int, power: str, time: int):
        self.feature = Effects[feature]
        self.points = points
        self.power = power
        self.time = time
        self.__effect = RogueEffect(self.feature, self.points, self.time)

    def __str__(self):
        return f"Зелье {self.feature.label} {self.power}"

    def __lt__(self, other: Item):
        return (
            self.power < other.power
            if self.feature.label == other.feature.label
            else self.feature.label < other.feature.label
        )
