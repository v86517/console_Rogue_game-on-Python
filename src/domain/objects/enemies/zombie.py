from pygame.mixer import Sound

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage

from ..utils import RogueEffect
from .enemy import Enemy


class Zombie(Enemy):
    def __init__(self, level: int):
        super().__init__("z", level)
        self.__hit_sound = SoundController.get_instance().get_sound(SoundType.Zombie, SoundUsage.hit)

    def attack(self) -> tuple[str, int, int, RogueEffect | None, Sound]:
        name, strength, agility, effect, _ = super().attack()
        return name, strength, agility, effect, self.__hit_sound
