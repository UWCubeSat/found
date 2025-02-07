import unittest
import numpy as np

from common.constants import EARTH_RADIUS
from spatial.coordinate import Attitude, Vector, CoordinateSystem
from curve.spherical import SphericalCurveProvider


class SphericalGeneratorTest(unittest.TestCase):
    def helper(self, attitude: Attitude, alpha_conj: float):
        TEST_POINTS = 10

        # Step 1: Generate Position Vector
        position = Vector(EARTH_RADIUS / np.cos(alpha_conj), 0, 0)

        # Step 2: Generate Circle Points
        center = Vector(EARTH_RADIUS * np.cos(alpha_conj), 0, 0)
        radius1 = Vector(0, EARTH_RADIUS * np.sin(alpha_conj), 0)
        radius2 = Vector(0, 0, EARTH_RADIUS * np.sin(alpha_conj))

        points = np.array(
            [
                (center + radius1 * np.cos(angle) + radius2 * np.sin(angle)).vector
                for angle in np.linspace(0, np.pi * 2, 720)
            ]
        )[np.random.randint(0, 720, TEST_POINTS)]

        # Rotate using a coodrinate system
        system = CoordinateSystem(attitude=attitude)
        rotation_matrix = np.array([vector.vector for vector in system.basis])
        expected_points = {
            Vector(numpy=vector) for vector in (rotation_matrix @ points.T).T
        }
        final_position = Vector(numpy=(rotation_matrix @ position.vector.T).T)
        expected_center = Vector(numpy=(rotation_matrix @ center.vector.T).T)
        radius1 = Vector(numpy=(rotation_matrix @ radius1.vector.T).T)
        radius2 = Vector(numpy=(rotation_matrix @ radius2.vector.T).T)

        # Create the SphericalCurveProvider and generate points
        curve_provider = SphericalCurveProvider(final_position)
        actual_center, actual_points = curve_provider.generate_points(720)

        # First, figure out if the basic vectors are correct
        self.assertEqual(expected_center, actual_center)
        normal_vector = radius1.cross(radius2)
        np.testing.assert_allclose(
            0,
            np.dot(normal_vector.vector, curve_provider.radius.vector)
            / (normal_vector.norm * curve_provider.radius.norm),
            atol=1e-6,
        )
        np.testing.assert_allclose(
            0,
            np.dot(normal_vector.vector, curve_provider.radius2.vector)
            / (normal_vector.norm * curve_provider.radius2.norm),
            atol=1e-6,
        )
        np.testing.assert_allclose(
            0,
            np.dot(curve_provider.radius.vector, curve_provider.radius2.vector),
            atol=1e-3,
        )
        np.testing.assert_allclose(
            EARTH_RADIUS * np.sin(alpha_conj), curve_provider.radius.norm, atol=1e-3
        )
        np.testing.assert_allclose(
            EARTH_RADIUS * np.sin(alpha_conj), curve_provider.radius2.norm, atol=1e-3
        )

        # Then, look at all points and see if the expected points are "in the array"
        tally = set()
        norms = []
        tolerance_angle = np.pi * 2 / 720 / 2
        tolerance_norm = (
            radius1.norm
            * np.sin(tolerance_angle)
            / np.sin((np.pi - tolerance_angle) / 2)
        )
        for actual_pt in actual_points:
            for expected_pt in expected_points:
                if (actual_pt - expected_pt).norm < tolerance_norm:
                    tally.add(expected_pt)
                norms.append(((actual_pt - expected_pt).norm))

        self.assertEqual(len(expected_points), len(tally))

    def test_no_rotation_small_angle(self):
        self.helper(Attitude(0, 0, 0), np.pi / 6)

    def test_no_rotation_big_angle(self):
        self.helper(Attitude(0, 0, 0), np.pi * 2 / 5)

    def test_simple_rotatation(self):
        self.helper(Attitude(np.pi / 4, 0, 0), np.pi / 9)

    def test_compound_rotatation(self):
        self.helper(Attitude(np.pi / 6, -np.pi / 3, 0), np.pi / 5)

    def test_full_rotations(self):
        self.helper(Attitude(np.pi / 2, -np.pi / 7, -np.pi / 11), np.pi / 100)
