from .coordinate import CoordinateSystem, Vector, Attitude

from typing import Iterable, Union, Tuple


class Camera(CoordinateSystem):
    """The camera represents a camera, complete with
    coordinate system information

    Parents:
        CoordinateSystem: Represents another 3D coordinate system
    """

    def __init__(
        self,
        position: Vector,
        orientation: Attitude,
        focal_length: float,
        pixel_size: float,
        x_resolution: float,
        y_resolution: float,
    ):
        """Creates a Camera

        Args:
            position (Vector): The position of this camera in space
            orientation (Attitude): The orientation of this camera
            focal_length (float): The focal length of this camera
            pixel_size (float): The pixel size of this camera
            x_resolution (float): The x resolution of this camera
            y_resolution (float): The y resolution of this camera
        """
        super().__init__(position, orientation)
        self.focal_length = focal_length
        self.pixel_length = pixel_size
        self.x_resolution = x_resolution
        self.y_resolution = y_resolution

    def to_camera_space(self, points: Iterable[Vector]) -> Iterable[Vector]:
        """Transforms vectors in the reference to the camera's frame

        Args:
            points (Iterable[Vector]): The points to transform

        Returns:
            Iterable[Vector]: The transformed points
        """
        return self.to_coordinate_system(points)

    def in_camera(self, point: Vector) -> bool:
        """Evaluates if a 2D point is in the camera
        sensor

        Args:
            point (Vector): The point to evaluate

        Raises:
            ValueError: If point is not a 2D point

        Returns:
            bool: True iff it is in the sensor
        """
        if point.dimension != 2:
            raise ValueError(
                f"Expected vector to have dimension 2, but was {point.dimension}"
            )
        x_lim = self.x_resolution / 2
        y_lim = self.y_resolution / 2
        return (-x_lim <= point[0] and point[0] <= x_lim) and (
            -y_lim <= point[1] and point[1] <= y_lim
        )

    def spatial_to_camera(
        self, *points: Iterable[Vector]
    ) -> Union[Vector, Tuple[None], Tuple[Vector]]:
        """Converts 3D points in the camera coordinate system to
        2D points on its sensor. Does not perform pixel truncation.

        Args:
            points: The points to transform

        Returns:
            Union[Vector, Tuple[None], Tuple[Vector]]: A single point if a single
            point was passed, a collection of None or a Tuple of Vectors corresponding
            to each point transformed

        Postcondition:
            Ignores points behind the sensor (are not included in the return value, and
            are dropped entirely).
            Does not ignore points that are not on the sensor but still in front of it.
        """
        result = []
        for point in points:
            if point[0] < self.focal_length:
                continue
            factor = self.focal_length / point[0] / self.pixel_length
            image_point = Vector(factor * point[1], factor * point[2])
            result.append(image_point)
            if self.in_camera(image_point):
                reference = Vector(self.x_resolution / 2, self.y_resolution / 2)
                canvas_pt = -image_point + reference
                print(f"{canvas_pt},")
        if not result:
            return [None] * len(points)
        elif len(result) == 1:
            return result[0]
        return result
