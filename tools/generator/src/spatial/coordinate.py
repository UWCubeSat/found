import numpy as np
from common.eval import float_equals
from typing import Iterable


class Attitude:
    def __init__(
        self,
        right_ascension: float,
        declination: float,
        roll: float,
        radians: bool = True,
    ):
        converter = lambda x: np.deg2rad(x) if not radians else x
        self.right_ascension = converter(right_ascension)
        self.declination = converter(declination)
        self.roll = converter(roll)

    def __str__(self):
        return f"({self.right_ascension:.2f}, {self.declination:.2f}, {self.roll:.2f})"


class Vector:
    def __init__(self, *values: float, numpy: np.ndarray = None):
        if numpy is None:
            self.dimension = len(values)
            self.vector = np.array(values)
        else:
            if len(numpy.shape) != 1:
                raise ValueError(
                    f"Numpy array is not of 1 dimension but is of shape {numpy.shape}"
                )
            self.dimension = len(numpy)
            self.vector = numpy

    def cross(self, vector):
        return Vector(self, numpy=np.cross(self.vector, vector.vector))

    @property
    def norm(self):
        return np.linalg.norm(self.vector)

    def normalize(self):
        return Vector(self, numpy=self.vector / self.norm)

    def __neg__(self):
        return Vector(numpy=-self.vector)

    def __add__(self, other):
        return Vector(numpy=self.vector + other.vector)

    def __sub__(self, other):
        return Vector(numpy=self.vector - other.vector)

    def __mul__(self, factor: float):
        return Vector(numpy=factor * self.vector)

    def __getitem__(self, index: int):
        return self.vector[index]

    def __iter__(self):
        return self.vector.__iter__()

    def __eq__(self, other):
        if not isinstance(other, Vector) or self.dimension != other.dimension:
            return False
        for x, y in zip(self.vector, other.vector):
            if not float_equals(x, y):
                return False
        return True

    def __hash__(self):
        return int(self.vector.sum().item())

    def __str__(self):
        result = ""
        for el in self.vector:
            result += f"{el:.2f}, "
        return f"({result[:-2]})"


class CoordinateSystem:
    def __init__(self, position: Vector = Vector(0, 0, 0), attitude: Attitude = None):
        if attitude is not None:
            ra = attitude.right_ascension
            de = attitude.declination  # Negate because this is CW
            roll = -attitude.roll  # Negate also because this is CW

            # Step 1: Form unit x vector with right ascension and declination
            # Formula: x_hat = <cos(de)cos(ra), cos(de)sin(ra), sin(de)>
            x_hat = Vector(np.cos(de) * np.cos(ra), np.cos(de) * np.sin(ra), np.sin(de))

            # Step 2: Form the nominal y vector with right ascension only
            y_hat_nominal = Vector(-np.sin(ra), np.cos(ra), 0)

            # Step 3: Form the nominal z vector by crossing the x and nominal y vectors
            z_hat_nominal = x_hat.cross(y_hat_nominal)

            # Step 4: Rotate z_hat vector according to roll in the plane of z_hat and y_hat.
            z_hat = z_hat_nominal * np.cos(roll) - y_hat_nominal * np.sin(roll)

            # Step 5: Rotate y_hat in a similar way
            y_hat = y_hat_nominal * np.cos(roll) + z_hat_nominal * np.sin(roll)

            # Step 6: Store it
            self.basis = np.array([x_hat, y_hat, z_hat])
        else:
            self.basis = np.array([Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)])

        # Step 7: Store the position
        self.position = position

    def to_coordinate_system(self, points: Iterable[Vector]):
        points = np.array([pt.vector - self.position.vector for pt in points])
        if len(points.shape) == 2 and not points.shape[0] == 3 and points.shape[1] == 3:
            points = points.T
        basis = np.array([axis.vector for axis in self.basis])
        transformed = np.linalg.solve(basis, points).T
        result = [Vector(numpy=point) for point in transformed]
        if len(result) == 1:
            return result[0]
        return result


CELESTIAL_SYSTEM = CoordinateSystem()
