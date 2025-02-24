import unittest
import numpy as np

from src.spatial.coordinate import Vector, Attitude
from src.curve.spherical import SphericalCurveProvider
from src.common.constants import (
    EARTH_RADIUS,
    NUM_EARTH_POINTS,
    DEFAULT_RESOLUTION,
)
from src.__main__ import generate_points


class IntegrationTest(unittest.TestCase):
    # Taking an inductive approach to this. First we manually calculate the circle we expect to see looking at the image head on, then we take those generated pixels that we know are correct, and then rotate them to get the other rotated images. Because we know our base pixels are correct and our rotation algorithm, then we can use them without worrying too much.

    def test_head_on(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(180, 0, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = 1  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        radius_len = (
            curve_provider.radius.norm
            * (focal_length * pixel_size)
            / (position.norm - curve_provider.center.norm)
        )
        r1, r2 = Vector(radius_len, 0), Vector(0, radius_len)

        expected_points = [
            r1 * np.sin(t) + r2 * np.cos(t) for t in np.linspace(0, 2 * np.pi, 10)
        ]
        theta_tolerance = np.pi / NUM_EARTH_POINTS  # Not 2pi because we half the angle
        dist_tolerance = r1.norm * theta_tolerance

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        np.testing.assert_allclose(radius_len, actual_points[0].norm)

        count = 0
        for exp_pt in expected_points:
            for act_pt in actual_points:
                if (exp_pt - act_pt).norm < dist_tolerance:
                    count += 1
                    break

        self.assertEqual(len(expected_points), count)
