from random import choice

from pygame.mixer import Sound

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage
from domain.objects.items import ITEMS
from domain.objects.utils import RogueEffect

from .enemy import Enemy, EnemyState


class Mimic(Enemy):
    def __init__(self, level: int):
        super().__init__("m", level)
        self.__hit_sound = SoundController.get_instance().get_sound(SoundType.Mimic, SoundUsage.hit)
        mimic_item = choice(ITEMS)
        self.mimic_color = mimic_item.color
        self.mimic_symbol = mimic_item.symbol

    def attack(self) -> tuple[str, int, int, RogueEffect | None, Sound]:
        name, strength, agility, effect, _ = super().attack()
        return name, strength, agility, effect, self.__hit_sound

    @property
    def color(self):
        if self._state != EnemyState.ENGAGED:
            return self.mimic_color
        return self._color

    @property
    def symbol(self):
        if self._state != EnemyState.ENGAGED:
            return self.mimic_symbol
        return self._symbol
