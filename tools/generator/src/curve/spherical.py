import numpy as np

from spatial.coordinate import Vector
from common.constants import EARTH_RADIUS, NUM_EARTH_POINTS
from common.eval import float_equals
from .curve import CurveProvider


class SphericalCurveProvider(CurveProvider):
    """
    Provides the edge points of earth by representing earth as a sphere

    Parents:
        CurveProvider: The base abstract class that defines a curve provider
    """

    def __init__(self, position: Vector):
        # Step 1: Get the center vector
        distance = position.norm
        alpha = np.arcsin(EARTH_RADIUS / distance)
        tangent_len = np.sqrt(distance * distance - EARTH_RADIUS * EARTH_RADIUS)
        center_len = distance - tangent_len * np.cos(alpha)
        self.center = position.normalize() * center_len

        # Step 2: Get the radius vector
        # Use the same vector actually to just manipulate it into an orthogonal one
        radius_len = tangent_len * np.sin(alpha)
        if not float_equals(0.0, position[0]):
            self.radius = Vector(-(position[1] + position[2]) / position[0], 1, 1)
        elif not float_equals(0.0, position[1]):
            self.radius = Vector(1, -(position[0] + position[2]) / position[1], 1)
        else:
            self.radius = Vector(1, 1, -(position[0] + position[1]) / position[2])
        self.radius = self.radius.normalize() * radius_len
        self.radius2 = self.center.cross(self.radius).normalize() * radius_len

    def generate_points(self, num_points: int = NUM_EARTH_POINTS):
        datapoints = np.linspace(0, 2 * np.pi, num_points)
        return self.center, [
            self.center + self.radius * np.sin(t) + self.radius2 * np.cos(t)
            for t in datapoints
        ]
