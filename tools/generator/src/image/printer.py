import cairo

from common.eval import float_equals
from spatial.coordinate import Vector
from spatial.camera import Camera

from typing import Iterable


class Printer:
    """A Printer prints an image of Earth"""

    def __init__(self, camera: Camera):
        """Creates this printer

        Args:
            camera (Camera): The camera that took the image
        """
        self.camera = camera
        self.width, self.height = camera.x_resolution, camera.y_resolution
        self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, self.width, self.height)
        self.context = cairo.Context(self.surface)

    def _start_drawing(self):
        """Sets up the printer to start drawing"""
        # Set background color (black)
        self.context.set_source_rgb(0, 0, 0)
        self.context.paint()

    def _transform_point(self, *points: Vector) -> Vector:
        """Transforms a point from the camera 2d axis to the
        graphics 2d axis

        Args:
            points (Vector): The points to transform

        Returns:
            Vector: The transformed points
        """
        result = []
        reference_point = Vector(
            self.camera.x_resolution / 2, self.camera.y_resolution / 2
        )
        for point in points:
            result.append(reference_point - point)
        return result

    def _draw_earth(self, curve_points: Iterable[Vector]):
        """Draws Earth given its curve points

        Args:
            curve_points (Iterable[Vector]): The curve points of Earth
            that are observed as its edge
        """
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
        """Stops drawing"""
        pass

    def generate_image(self, curve_points: Iterable[Vector]):
        """Generates an image of earth

        Args:
            curve_points (Iterable[Vector]): The edge points of earth
            to use
        """
        self._start_drawing()
        self._draw_earth(curve_points)
        self._stop_drawing()

    def save_image(self, filename: str):
        """Saves the generated image of earth

        Args:
            filename (str): The file to save it into

        Precondition
            self.generate_image(...) should've been
            called before this
        """
        if '.png' in filename:
            filename = filename[:filename.index('.png')]
        self.surface.write_to_png(f"{filename}.png")
