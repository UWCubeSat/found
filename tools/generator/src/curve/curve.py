from abc import ABC, abstractmethod

from spatial.coordinate import Vector


class CurveProvider(ABC):
    @abstractmethod
    def __init__(self, position: Vector):
        pass

    @abstractmethod
    def generate_points(self, num_points: int = 360):
        pass
