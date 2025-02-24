import unittest
import numpy as np

from src.spatial.camera import Camera
from src.spatial.coordinate import Attitude, Vector


class CameraTest(unittest.TestCase):
    """Camera Vector Transformation"""

    def test_random_transformations(self):
        TRANSFORMATION_TESTS = 100
        NUM_POINTS = 50
        POS_LIM = 500
        RESOLUTION = 1024

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
            system = Camera(
                pos,
                att,
                np.random.rand(),
                1,
                RESOLUTION,
                RESOLUTION,
            )

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

    """ in_camera """

    def test_in_camera_true(self):
        TESTS = 100
        NUM_POINTS = 50
        POS_LIM = 500
        RESOLUTION_LIM_LO = 3
        RESOLUTION_LIM_HI = 10
        FOCAL_LEN_LIM_LO = 1e-6
        FOCAL_LEN_LIM_HI = 1e-2
        FOCAL_LEN_RANGE = FOCAL_LEN_LIM_HI - FOCAL_LEN_LIM_LO

        attitudes = [
            Attitude(*(np.random.rand(3) * np.pi * 4 - np.pi * 2)) for _ in range(TESTS)
        ]
        positions = [
            Vector(*(np.random.rand(3) * POS_LIM * 2 - POS_LIM)) for _ in range(TESTS)
        ]
        focal_lengths = [
            np.random.rand() * FOCAL_LEN_RANGE + FOCAL_LEN_LIM_LO for _ in range(TESTS)
        ]
        pixel_sizes = [np.random.rand() for _ in range(TESTS)]
        x_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]
        y_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]

        parameters = [
            attitudes,
            positions,
            focal_lengths,
            pixel_sizes,
            x_resolutions,
            y_resolutions,
        ]

        points = [
            [
                Vector(point[0] * x_res - 1e-6, point[1] * y_res - 1e-6)
                - Vector(x_res / 2, y_res / 2)
                for point in group
            ]
            for group, x_res, y_res in zip(
                np.random.rand(TESTS, NUM_POINTS, 2), x_resolutions, y_resolutions
            )
        ]

        for att, pos, f, pix_size, x_res, y_res, pts in zip(*parameters, points):
            cam = Camera(pos, att, f, pix_size, x_res, y_res)

            for pt in pts:
                self.assertTrue(cam.in_camera(pt))

    def test_in_camera_false(self):
        TESTS = 100
        NUM_POINTS = 50
        POS_LIM = 500
        RESOLUTION_LIM_LO = 3
        RESOLUTION_LIM_HI = 10
        FOCAL_LEN_LIM_LO = 1e-6
        FOCAL_LEN_LIM_HI = 1e-2
        FOCAL_LEN_RANGE = FOCAL_LEN_LIM_HI - FOCAL_LEN_LIM_LO

        attitudes = [
            Attitude(*(np.random.rand(3) * np.pi * 4 - np.pi * 2)) for _ in range(TESTS)
        ]
        positions = [
            Vector(*(np.random.rand(3) * POS_LIM * 2 - POS_LIM)) for _ in range(TESTS)
        ]
        focal_lengths = [
            np.random.rand() * FOCAL_LEN_RANGE + FOCAL_LEN_LIM_LO for _ in range(TESTS)
        ]
        pixel_sizes = [np.random.rand() for _ in range(TESTS)]
        x_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]
        y_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]

        parameters = [
            attitudes,
            positions,
            focal_lengths,
            pixel_sizes,
            x_resolutions,
            y_resolutions,
        ]

        points = [
            [
                Vector(point[0] * x_res, point[1] * y_res)
                + Vector(np.sign(point[0]) * x_res / 2, np.sign(point[1]) * y_res / 2)
                for point in group
            ]
            for group, x_res, y_res in zip(
                np.random.rand(TESTS, NUM_POINTS, 2), x_resolutions, y_resolutions
            )
        ]

        for att, pos, f, pix_size, x_res, y_res, pts in zip(*parameters, points):
            cam = Camera(pos, att, f, pix_size, x_res, y_res)

            for pt in pts:
                self.assertFalse(cam.in_camera(pt))

    """ Spatial to Camera (3D to 2D Projection) """

    def test_spatial_to_camera_single(self):
        position = Vector(10000, 0, 20000)
        attitude = Attitude(1.0, 2.0, -3.0)
        focal_length = 0.03
        x_resolution, y_resolution = 1024, 1024

        cam = Camera(position, attitude, focal_length, 0.5, x_resolution, y_resolution)

        testVector = Vector(1, 2, 3)
        expectedVector = Vector(
            2 * 2 * focal_length, 3 * 2 * focal_length
        )  # f/x, but x is 1, then pixel size scales the coordinates by 2

        actualVector = cam.spatial_to_camera(testVector)

        self.assertEqual(expectedVector, actualVector)

    def test_spatial_to_camera(self):
        TESTS = 100
        NUM_POINTS = 50
        POS_LIM = 500
        RESOLUTION_LIM_LO = 3
        RESOLUTION_LIM_HI = 10
        FOCAL_LEN_LIM_LO = 1e-6
        FOCAL_LEN_LIM_HI = 1e-2
        FOCAL_LEN_RANGE = FOCAL_LEN_LIM_HI - FOCAL_LEN_LIM_LO

        attitudes = [
            Attitude(*(np.random.rand(3) * np.pi * 4 - np.pi * 2)) for _ in range(TESTS)
        ]
        positions = [
            Vector(*(np.random.rand(3) * POS_LIM * 2 - POS_LIM)) for _ in range(TESTS)
        ]
        focal_lengths = [
            np.random.rand() * FOCAL_LEN_RANGE + FOCAL_LEN_LIM_LO for _ in range(TESTS)
        ]
        x_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]
        y_resolutions = [
            np.exp2(np.random.randint(RESOLUTION_LIM_LO, RESOLUTION_LIM_HI))
            for _ in range(TESTS)
        ]

        parameters = [attitudes, positions, focal_lengths, x_resolutions, y_resolutions]

        expected_points = [
            [
                Vector(point[0] * 2 * x_res - 1e-6, point[1] * 2 * y_res - 1e-6)
                - Vector(x_res / 2, y_res / 2)
                for point in group
            ]
            for group, x_res, y_res in zip(
                np.random.rand(TESTS, NUM_POINTS, 2), x_resolutions, y_resolutions
            )
        ]

        projected_points = [
            [Vector(x, x / f * point[0], x / f * point[1]) for point in group]
            for group, x, f in zip(
                expected_points, np.random.rand(len(expected_points)), focal_lengths
            )
        ]

        for exp_pts, proj_pts, att, pos, f, x_res, y_res in zip(
            expected_points, projected_points, *parameters
        ):
            cam = Camera(pos, att, f, 1, x_res, y_res)

            actual_pts = cam.spatial_to_camera(*proj_pts)
            if None in actual_pts:
                continue
            self.assertListEqual(exp_pts, actual_pts)

    # TODO (nguy8tri): Need to figure out what we're going to do when x is negative
