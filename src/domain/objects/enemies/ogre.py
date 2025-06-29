from pygame.mixer import Sound

from controller.sound.sound_controller import SoundController, SoundType, SoundUsage

from ..utils import Effects, RogueEffect
from .enemy import Enemy


class Ogre(Enemy):
    def __init__(self, level: int):
        super().__init__("O", level)
        self.__hit_sound = SoundController.get_instance().get_sound(SoundType.Ogre, SoundUsage.hit)

    def attack(self) -> tuple[str, int, int, RogueEffect | None, Sound]:
        self._effects[Effects.SLEEP].append(RogueEffect(Effects.SLEEP, 0, 1))
        name, strength, agility, effect, _ = super().attack()
        return name, strength, agility, effect, self.__hit_sound
