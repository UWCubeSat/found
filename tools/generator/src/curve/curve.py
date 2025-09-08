from abc import ABC, abstractmethod

from common.constants import NUM_EARTH_POINTS
from spatial.coordinate import Vector

from typing import Tuple, Iterable


class CurveProvider(ABC):
    """Provides the edge points in the celestial coordinate system as visible from a satelite at some position"""

    @abstractmethod
    def __init__(self, position: Vector):
        """Initializes this curve provider

        Args:
            position (Vector): The position vector that represents where the satellite is with relation to Earth, in the celestial coordinate system
        """
        pass

    @abstractmethod
    def generate_points(
        self, num_points: int = NUM_EARTH_POINTS
    ) -> Tuple[Vector, Iterable[Vector]]:
        """Generates the visible edge points of earth as can be possibly observed
        from a spacecraft, in terms of the celestial coordinate system

        Args:
            num_points (int, optional): The number of points to generate. Defaults to NUM_EARTH_POINTS.

        Returns:
            Tuple[Vector, Iterable[Vector]]: The center of the points, and the edge points.
        """
        pass
