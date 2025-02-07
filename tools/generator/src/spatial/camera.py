from .coordinate import CoordinateSystem, Vector, Attitude

from typing import Iterable


class Camera(CoordinateSystem):
    def __init__(
        self,
        position: Vector,
        orientation: Attitude,
        focal_length: float,
        pixel_size: float,
        x_resolution: float,
        y_resolution: float,
    ):
        super().__init__(position, orientation)
        print(*self.basis)
        self.focal_length = focal_length
        self.pixel_length = pixel_size
        self.x_resolution = x_resolution
        self.y_resolution = y_resolution

    def to_camera_space(self, points: Iterable[Vector]):
        return self.to_coordinate_system(points)

    def in_camera(self, point: Vector):
        if point.dimension != 2:
            raise ValueError(
                f"Expected vector to have dimension 2, but was {point.dimension}"
            )
        x_lim = self.x_resolution / 2
        y_lim = self.y_resolution / 2
        return (-x_lim <= point[0] and point[0] <= x_lim) and (
            -y_lim <= point[1] and point[1] <= y_lim
        )

    def spatial_to_camera(self, *points: Iterable[Vector]):
        # TODO (nguy8tri): Need to figure out what we're going to do when x is negative
        result = []
        for point in points:
            if point[0] < self.focal_length:
                continue
            factor = self.focal_length / point[0] / self.pixel_length
            image_point = Vector(factor * point[1], factor * point[2])
            result.append(image_point)
        if not result:
            return [None] * len(points)
        elif len(result) == 1:
            return result[0]
        return result
