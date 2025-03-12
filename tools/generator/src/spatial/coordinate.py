import numpy as np
from common.eval import float_equals
from typing import Iterable, Iterator


class Attitude:
    """Represents an attitude in the celestial sphere (using modified Euler Angles)"""

    def __init__(
        self,
        right_ascension: float,
        declination: float,
        roll: float,
        radians: bool = True,
    ):
        """Creates an Attitude object

        Args:
            right_ascension (float): The right ascension of the attitude
            declination (float): The declination of the attitude
            roll (float): The roll of the attitude
            radians (bool, optional): Whether the 3 parameters are all specified as radians. Defaults to True.
        """
        converter = lambda x: np.deg2rad(x) if not radians else x
        self.right_ascension = converter(right_ascension)
        self.declination = converter(declination)
        self.roll = converter(roll)

    def __str__(self) -> str:
        """Returns the string representation of this

        Returns:
            str: The string that represents this
        """
        return f"({self.right_ascension:.2f}, {self.declination:.2f}, {self.roll:.2f})"


class Vector:
    """Represents a R^n Vector"""

    def __init__(self, *values: Iterable[float], numpy: np.ndarray = None):
        """Creates a vector

        Args:
            values (Iterable[float]): The values to give this vector
            numpy (np.ndarray, optional): A numpy array to use for this Vector. Defaults to None.

        Raises:
            ValueError: If the values or numpy array given can't be an array
        """
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
        """Returns the cross product of this with some other vector

        Args:
            vector (Vector): The vector to cross this with

        Returns:
            Vector: The cross of this and vector
        """
        return Vector(self, numpy=np.cross(self.vector, vector.vector))

    @property
    def norm(self) -> float:
        """Is the norm of this

        Returns:
            float: The norm of this
        """
        return np.linalg.norm(self.vector)

    def normalize(self):
        """Normalizes this

        Returns:
            Vector: this, but with a length of one
        """
        return Vector(self, numpy=self.vector / self.norm)

    def __neg__(self):
        """Negates this vector

        Returns:
            Vector: The negation of this
        """
        return Vector(numpy=-self.vector)

    def __add__(self, other):
        """Returns the sum of this with some other vector

        Args:
            vector (Vector): The vector to sum this with

        Returns:
            Vector: The sum of this and vector
        """
        return Vector(numpy=self.vector + other.vector)

    def __sub__(self, other):
        """Returns the difference between this and some other vector

        Args:
            vector (Vector): The vector to subtract this from

        Returns:
            Vector: The difference between this and vector
        """
        return Vector(numpy=self.vector - other.vector)

    def __mul__(self, factor: float):
        """Returns the product of this with some other vector

        Args:
            vector (Vector): The vector to multiple this with

        Returns:
            Vector: The product of this and vector
        """
        return Vector(numpy=factor * self.vector)

    def __getitem__(self, index: int):
        """Obtains an element of this

        Args:
            index (int): The index of the element to get

        Returns:
            float: The value of the element at index
        """
        return self.vector[index]

    def __iter__(self) -> Iterator[float]:
        """Returns the iteration object for this

        Returns:
            Iterator[float]: The iteration object for this
        """
        return self.vector.__iter__()

    def __eq__(self, other) -> bool:
        """Evaluates if this vector is the same as the other vector

        Args:
            other (Any): The vector to cross this with

        Returns:
            bool: If this has the same contents as other
        """
        if not isinstance(other, Vector) or self.dimension != other.dimension:
            return False
        for x, y in zip(self.vector, other.vector):
            if not float_equals(x, y):
                return False
        return True

    def __hash__(self) -> int:
        """Returns the hash of this

        Returns:
            int: The hash of this
        """
        return int(self.vector.sum().item())

    def __str__(self) -> str:
        """Returns the string representation of this

        Returns:
            str: The string of this
        """
        result = ""
        for el in self.vector:
            result += f"(decimal) {el:.64f}, "
        return f"/{{result[:-2]}/}"


class CoordinateSystem:
    """Represents a 3D coordinate System"""

    def __init__(self, position: Vector = Vector(0, 0, 0), attitude: Attitude = None):
        """Initializes this coordinate system with a position and rotation

        Args:
            position (Vector, optional): The position of the origin. Defaults to Vector(0, 0, 0).
            attitude (Attitude, optional): The rotation applied to this coordinate system. Defaults to None.
        """
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

    def to_coordinate_system(self, points: Iterable[Vector]) -> Iterable[Vector]:
        """Transforms vectors in the reference to the coordinate system's frame

        Args:
            points (Iterable[Vector]): The points to transform

        Returns:
            Iterable[Vector]: The transformed points
        """
        points = np.array([pt.vector - self.position.vector for pt in points])
        if len(points.shape) == 2 and not points.shape[0] == 3 and points.shape[1] == 3:
            points = points.T
        basis = np.column_stack([axis.vector for axis in self.basis])
        transformed = np.linalg.solve(basis, points).T
        result = [Vector(numpy=point) for point in transformed]
        if len(result) == 1:
            return result[0]
        return result


CELESTIAL_SYSTEM = CoordinateSystem()
