from copy import copy
from random import random

from pygame.mixer import Sound

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage
from domain.objects.utils import RogueEffect, RogueEvent

from .enemy import Effects, Enemy


class Vampire(Enemy):
    __DIMINISHED_HEALTH_PROP = 0.2

    def __init__(self, level: int):
        super().__init__("v", level)
        self.__first_hit = True
        self.__effect = RogueEffect(Effects.MAX_HEALTH, -(level * 4), 5)
        self.__hit_sound = SoundController.get_instance().get_sound(SoundType.Vampire, SoundUsage.hit)

    def add_attack_effect(self) -> RogueEffect | None:
        return None if random() > self.__DIMINISHED_HEALTH_PROP else copy(self.__effect)

    def harm(
        self, e_name: str, e_strength: int, e_agility, e_effect: RogueEffect, sound: Sound
    ) -> tuple[list[RogueEvent], int]:
        if self.__first_hit:
            e_agility = 0
            self.__first_hit = False
        return super().harm(e_name, e_strength, e_agility, e_effect, sound)

    def set_engaged_status(self) -> None:
        if not super().status_engaged():
            self.__first_hit = True
        super().set_engaged_status()

    def attack(self) -> tuple[str, int, int, RogueEffect | None, Sound]:
        name, strength, agility, effect, _ = super().attack()
        return name, strength, agility, effect, self.__hit_sound
