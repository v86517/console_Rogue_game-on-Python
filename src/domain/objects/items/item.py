import math
import random
from abc import ABC, abstractmethod
from typing import Any


class Item(ABC):
    type = "предмет"
    symbol = "?"
    color = 8

    @property
    @abstractmethod
    def effect(self):
        """Эффект, получаемый с предмета"""
        raise NotImplementedError

    @staticmethod
    def _random_type(level: int, item_types, base_weights, alpha_values) -> tuple[Any, Any]:  # TODO типы!
        target = 0.5

        weights = {
            item: base_weights[item] * math.exp(-alpha_values[item] * (level - 1))
            + target * (1 - math.exp(-alpha_values[item] * (level - 1)))
            for item in item_types
        }

        total_weight = sum(weights.values())
        normalized_weights = {item: weight / total_weight for item, weight in weights.items()}

        items = list(normalized_weights.keys())
        probabilities = list(normalized_weights.values())
        chosen_item = random.choices(items, probabilities, k=1)[0]

        return chosen_item, item_types[chosen_item]

    @abstractmethod
    def pick_up(self):
        """Вывод информации о поднятом предмете"""
        raise NotImplementedError

    @abstractmethod
    def drop(self):
        """Вывод информации о сброшенном предмете"""
        raise NotImplementedError

    @abstractmethod
    def use(self):
        """Вывод информации об использованном предмете"""
        raise NotImplementedError

    @abstractmethod
    def __str__(self):
        """Возвращает строковое представление предмета"""
        raise NotImplementedError

    @abstractmethod
    def __lt__(self, other):
        """Сравнение предметов на основе типа и характеристик"""
        raise NotImplementedError
