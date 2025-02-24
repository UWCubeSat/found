from src.spatial.coordinate import Vector, CoordinateSystem, Attitude, CELESTIAL_SYSTEM
import unittest
import numpy as np


class CoordinateTests(unittest.TestCase):
    """Constructor Tests"""

    def test_normal_basis(self):
        x_hat, y_hat, z_hat = CELESTIAL_SYSTEM.basis

        self.assertEqual(Vector(1, 0, 0), x_hat)
        self.assertEqual(Vector(0, 1, 0), y_hat)
        self.assertEqual(Vector(0, 0, 1), z_hat)

    # Answers from https://www.andre-gaschler.com/rotationconverter/

    def test_ra_only_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(np.pi / 2, 0, 0)).basis

        self.assertEqual(Vector(0, 1, 0), x_hat)
        self.assertEqual(Vector(-1, 0, 0), y_hat)
        self.assertEqual(Vector(0, 0, 1), z_hat)

    def test_de_only_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(0, 5.3, 0)).basis

        self.assertEqual(Vector(0.5543743, 0, -0.8322675), x_hat)
        self.assertEqual(Vector(0, 1, 0), y_hat)
        self.assertEqual(Vector(0.8322675, 0, 0.5543743), z_hat)

    def test_roll_only_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(0, 0, 2.9)).basis

        self.assertEqual(Vector(1, 0, 0), x_hat)
        self.assertEqual(Vector(0, -0.9709582, -0.2392493), y_hat)
        self.assertEqual(Vector(0, 0.2392493, -0.9709582), z_hat)

    def test_ra_de_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(
            attitude=Attitude(np.pi / 4, np.pi / 4, 0)
        ).basis

        self.assertEqual(Vector(0.5, 0.5, 0.7071068), x_hat)
        self.assertEqual(Vector(-0.7071068, 0.7071068, 0.0), y_hat)
        self.assertEqual(Vector(-0.5, -0.5, 0.7071068), z_hat)

    def test_ra_roll_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(-4, 0, 5)).basis

        self.assertEqual(Vector(-0.6536436, 0.7568025, 0), x_hat)
        self.assertEqual(Vector(-0.2146762, -0.1854140, 0.9589243), y_hat)
        self.assertEqual(Vector(0.7257163, 0.6267948, 0.2836622), z_hat)

    def test_dec_roll_rotation(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(0, 3.7, -4.5)).basis

        self.assertEqual(Vector(-0.8481000, 0, -0.5298361), x_hat)
        self.assertEqual(Vector(-0.5179308, -0.2107958, 0.8290433), y_hat)
        self.assertEqual(Vector(-0.1116872, 0.9775301, 0.1787759), z_hat)

    def test_general_initialization(self):
        x_hat, y_hat, z_hat = CoordinateSystem(attitude=Attitude(0.5, 1.5, 4.5)).basis

        self.assertEqual(Vector(0.0620777, 0.0339132, 0.9974950), x_hat)
        self.assertEqual(Vector(-0.7546535, -0.6524696, 0.0691477), y_hat)
        self.assertEqual(Vector(0.6531802, -0.7570556, -0.0149111), z_hat)

    """ Transformation (Into System) Tests """

    # def test_single_transformation(self):
    #     attitude = Attitude(np.)

    def test_random_transformations(self):
        TRANSFORMATION_TESTS = 100
        NUM_POINTS = 50

        POS_LIM = 500

        attitudes = [
            Attitude(*(np.random.rand(3) * np.pi * 4 - np.pi * 2))
            for _ in range(TRANSFORMATION_TESTS)
        ]
        positions = [
            Vector(*(np.random.rand(3) * POS_LIM * 2 - POS_LIM))
            for _ in range(TRANSFORMATION_TESTS)
        ]
        points = [
            [Vector(numpy=arr) for arr in group]
            for group in np.random.rand(TRANSFORMATION_TESTS, NUM_POINTS, 3)
        ]

        for (att, pos), pts in zip(zip(attitudes, positions), points):
            system = CoordinateSystem(pos, att)

            expected_points = [
                Vector(numpy=expected_point)
                for expected_point in (
                    np.linalg.solve(
                        np.array([axis.vector for axis in system.basis]),
                        np.array([arr.vector - pos.vector for arr in pts]).T,
                    ).T
                )
            ]

            actual_points = system.to_coordinate_system(pts)

            self.assertEqual(len(expected_points), len(actual_points))
            self.assertListEqual(expected_points, actual_points)
