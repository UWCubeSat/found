import unittest
from unittest.mock import patch
import numpy as np

from src.spatial.coordinate import Vector, Attitude, CoordinateSystem
from src.curve.spherical import SphericalCurveProvider
from src.common.constants import EARTH_RADIUS, DEFAULT_RESOLUTION, DEFAULT_PIXEL_SIZE
from src.__main__ import generate_points


class IntegrationTest(unittest.TestCase):
    # Taking an inductive approach to this. First we manually calculate the circle we expect to see looking at the image head on, then we take those generated pixels that we know are correct, and then rotate them to get the other rotated images. Because we know our base pixels are correct and our rotation algorithm, then we can use them without worrying too much.

    def setUp(self):
        import src.common.constants as constants

        constants.NUM_EARTH_POINTS = 360
        self.NUM_EARTH_POINTS = constants.NUM_EARTH_POINTS

    def test_approx_right_rotation(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(180 - 25, 0, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        left = 0
        for pt in actual_points:
            if pt[0] >= 0:
                left += 1
        self.assertGreater(left, len(actual_points) - left)

    def test_approx_left_rotation(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(180 + 25, 0, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        left = 0
        for pt in actual_points:
            if pt[0] >= 0:
                left += 1
        self.assertLess(left, len(actual_points) - left)

    def test_approx_up_rotation(self):
        position = Vector(-EARTH_RADIUS - 1000000, 0, 0)
        orientation = Attitude(0, -25, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        up = 0
        for pt in actual_points:
            if pt[1] >= 0:
                up += 1
        self.assertGreater(up, len(actual_points) - up)

    def test_approx_down_rotation(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(180, 25, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        up = 0
        for pt in actual_points:
            if pt[1] >= 0:
                up += 1
        self.assertLess(up, len(actual_points) - up)

    def test_approx_inplace_rotation(self):
        position = Vector(-EARTH_RADIUS - 1000000, 0, 0)
        orientation = Attitude(0, 0, 59, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
        )

        up = 0
        left = 0
        for pt in actual_points:
            if pt[0] >= 0:
                left += 1
            if pt[1] >= 0:
                up += 1
        self.assertAlmostEqual(up, len(actual_points) - up)
        self.assertAlmostEqual(left, len(actual_points) - left)

    @patch("src.__main__.exit", side_effect=RuntimeError("Expected"))
    def test_cannot_see_earth(self, _):
        position = Vector(EARTH_RADIUS + 10000, 0, 0)
        leo_threshold = 2000000
        focal_length = 0.085
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        for orientation in [
            Attitude(0, 0, 0, radians=False),
            Attitude(45, 10, 0, radians=False),
            Attitude(45, 20, 10, radians=False),
        ]:
            with self.assertRaises(RuntimeError):
                generate_points(
                    position,
                    orientation,
                    leo_threshold,
                    focal_length,
                    pixel_size,
                    x_resolution,
                    y_resolution,
                    num_points=self.NUM_EARTH_POINTS,
                )

    def test_exact_head_on(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(180, 0, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        radius_len = (
            curve_provider.radius.norm
            * focal_length
            / (position.norm - curve_provider.center.norm)
            / pixel_size
        )
        r1, r2 = Vector(radius_len, 0), Vector(0, radius_len)

        expected_points = [
            r1 * np.sin(t) + r2 * np.cos(t) for t in np.linspace(0, 2 * np.pi, 20)
        ]
        theta_tolerance = (
            np.pi / self.NUM_EARTH_POINTS
        )  # Not 2pi because we half the angle
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

    def test_rotation_simple(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(145, 0, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        center = curve_provider.center.normalize() * (
            position.norm - curve_provider.center.norm
        )
        r1, r2 = curve_provider.radius, curve_provider.radius2
        rotation = CoordinateSystem(
            attitude=Attitude(-(180 - 145), 0, 0, radians=False)
        )

        expected_points = rotation.to_coordinate_system(
            [
                center + r1 * np.sin(t) + r2 * np.cos(t)
                for t in np.linspace(0, 2 * np.pi, self.NUM_EARTH_POINTS)
            ]
        )

        expected_points = [
            (Vector(vec[1], vec[2]) * (focal_length / vec[0] / pixel_size))
            for vec in expected_points
            if vec[0] > focal_length
        ]

        dist_tolerance = 0
        for i in range(len(expected_points) - 1):
            dist_tolerance = max(
                dist_tolerance, (expected_points[i] - expected_points[i + 1]).norm / 2
            )

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
            num_points=self.NUM_EARTH_POINTS,
        )

        # All points should be visible
        self.assertAlmostEqual(len(expected_points), len(actual_points), delta=1)

        count = 0
        for exp_pt in expected_points:
            for act_pt in actual_points:
                if (exp_pt - act_pt).norm < dist_tolerance:
                    count += 1
                    break

        self.assertAlmostEqual(len(expected_points), count, delta=1)

    def test_rotation_two(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(145, 20, 0, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        center = curve_provider.center.normalize() * (
            position.norm - curve_provider.center.norm
        )
        r1, r2 = curve_provider.radius, curve_provider.radius2
        rotation = CoordinateSystem(
            attitude=Attitude(-(180 - 145), -20, 0, radians=False)
        )

        expected_points = rotation.to_coordinate_system(
            [
                center + r1 * np.sin(t) + r2 * np.cos(t)
                for t in np.linspace(0, 2 * np.pi, self.NUM_EARTH_POINTS)
            ]
        )

        expected_points = [
            (Vector(vec[1], vec[2]) * (focal_length / vec[0] / pixel_size))
            for vec in expected_points
            if vec[0] > focal_length
        ]

        dist_tolerance = 0
        for i in range(len(expected_points) - 1):
            dist_tolerance = max(
                dist_tolerance, (expected_points[i] - expected_points[i + 1]).norm / 2
            )

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
            num_points=self.NUM_EARTH_POINTS,
        )

        # All points should be visible
        self.assertAlmostEqual(len(expected_points), len(actual_points), delta=1)

        count = 0
        for exp_pt in expected_points:
            for act_pt in actual_points:
                if (exp_pt - act_pt).norm < dist_tolerance:
                    count += 1
                    break

        self.assertAlmostEqual(len(expected_points), count, delta=1)

    def test_rotation_three(self):
        position = Vector(EARTH_RADIUS + 1000000, 0, 0)
        orientation = Attitude(145, 20, -60, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        center = curve_provider.center.normalize() * (
            position.norm - curve_provider.center.norm
        )
        r1, r2 = curve_provider.radius, curve_provider.radius2
        rotation = CoordinateSystem(
            attitude=Attitude(-(180 - 145), -20, 60, radians=False)
        )

        expected_points = rotation.to_coordinate_system(
            [
                center + r1 * np.sin(t) + r2 * np.cos(t)
                for t in np.linspace(0, 2 * np.pi, self.NUM_EARTH_POINTS)
            ]
        )

        expected_points = [
            (Vector(vec[1], vec[2]) * (focal_length / vec[0] / pixel_size))
            for vec in expected_points
            if vec[0] > focal_length
        ]

        dist_tolerance = 0
        for i in range(len(expected_points) - 1):
            dist_tolerance = max(
                dist_tolerance, (expected_points[i] - expected_points[i + 1]).norm / 2
            )

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
            num_points=self.NUM_EARTH_POINTS,
        )

        # All points should be visible
        self.assertAlmostEqual(len(expected_points), len(actual_points), delta=1)

        count = 0
        for exp_pt in expected_points:
            for act_pt in actual_points:
                if (exp_pt - act_pt).norm < dist_tolerance:
                    count += 1
                    break

        self.assertAlmostEqual(len(expected_points), count, delta=1)

    def test_general(self):
        nominal_system = CoordinateSystem(
            attitude=Attitude(180 - 45, -45, -90, radians=False)
        )
        position = -nominal_system.basis[0] * (EARTH_RADIUS + 1000000)
        orientation = Attitude(180 - 45 + 45, -45 - 20, -90 + 81, radians=False)
        leo_threshold = EARTH_RADIUS + 1
        focal_length = 0.012
        pixel_size = DEFAULT_PIXEL_SIZE  # For convenience
        x_resolution = DEFAULT_RESOLUTION
        y_resolution = DEFAULT_RESOLUTION

        curve_provider = SphericalCurveProvider(position)
        coordinate_system = CoordinateSystem(attitude=orientation)
        expected_points = curve_provider.generate_points(self.NUM_EARTH_POINTS)[1]

        # Let it do the transformation, I'm stumped as to why me manually doing it
        # doesn't work (with relative transformations)
        expected_points = coordinate_system.to_coordinate_system(
            [pt - position for pt in expected_points]
        )

        expected_points = [
            (Vector(vec[1], vec[2]) * (focal_length / vec[0] / pixel_size))
            for vec in expected_points
            if vec[0] > focal_length
        ]

        dist_tolerance = 0
        for i in range(len(expected_points) - 1):
            dist_tolerance = max(
                dist_tolerance, (expected_points[i] - expected_points[i + 1]).norm / 2
            )
        # self.fail(dist_tolerance)

        camera, actual_points = generate_points(
            position,
            orientation,
            leo_threshold,
            focal_length,
            pixel_size,
            x_resolution,
            y_resolution,
            num_points=self.NUM_EARTH_POINTS,
        )

        # All points should be visible
        self.assertAlmostEqual(len(expected_points), len(actual_points), delta=1)

        count = 0
        for exp_pt in expected_points:
            for act_pt in actual_points:
                if (exp_pt - act_pt).norm < dist_tolerance:
                    count += 1
                    break

        self.assertAlmostEqual(len(expected_points), count, delta=1)
