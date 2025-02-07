import cairo

from common.eval import float_equals
from spatial.coordinate import Vector
from spatial.camera import Camera

from typing import Iterable


class Printer:
    def __init__(self, camera: Camera):
        self.camera = camera
        self.width, self.height = camera.x_resolution, camera.y_resolution
        self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, self.width, self.height)
        self.context = cairo.Context(self.surface)

    def _start_drawing(self):
        # Set background color (black)
        self.context.set_source_rgb(0, 0, 0)
        self.context.paint()

    def _transform_point(self, *points: Vector):
        result = []
        reference_point = Vector(
            self.camera.x_resolution / 2, self.camera.y_resolution / 2
        )
        for point in points:
            result.append(reference_point - point)
        return result

    def _draw_earth(self, curve_points: Iterable[Vector]):
        # Set fill color (RGB: Blue)
        self.context.set_source_rgb(0, 50, 100)
        curve_points = self._transform_point(*curve_points)

        # Move to the first point
        self.context.move_to(*curve_points[0])

        for point in curve_points[1:]:
            self.context.line_to(*point)

        # Close the shape and fill it
        self.context.close_path()
        self.context.fill()

    def _stop_drawing(self):
        pass

    def generate_image(self, curve_points: Iterable[Vector]):
        self._start_drawing()
        self._draw_earth(curve_points)
        self._stop_drawing()

    def save_image(self, filename: str):
        self.surface.write_to_png(f"{filename}.png")
