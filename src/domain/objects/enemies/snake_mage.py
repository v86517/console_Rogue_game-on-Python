from copy import copy
from random import random

from pygame.mixer import Sound

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage
from domain.objects.utils import RogueEffect
from utils.logger import domain_log

from .enemy import Effects, Enemy


class SnakeMage(Enemy):
    __SLEEP_PROP = 0.2

    def __init__(self, level: int):
        super().__init__("s", level)
        self.__effect = RogueEffect(Effects.SLEEP, 0, (level // 7 or 1))
        self.__hit_sound = SoundController.get_instance().get_sound(SoundType.SnakeMage, SoundUsage.hit)

    def add_attack_effect(self) -> RogueEffect | None:
        domain_log.debug(f"{self.__class__.__name__} adding effect {self.__effect}")
        return None if random() > self.__SLEEP_PROP else copy(self.__effect)

    def attack(self) -> tuple[str, int, int, RogueEffect | None, Sound]:
        name, strength, agility, effect, _ = super().attack()
        return name, strength, agility, effect, self.__hit_sound
