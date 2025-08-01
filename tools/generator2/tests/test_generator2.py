#!/usr/bin/env python3
"""
Test suite for the Generator2 tool.

This module provides basic tests to validate the modular components
and ensure the tool functions correctly.
"""

import unittest
import numpy as np
from pathlib import Path
import tempfile
import os

# Import the modules to test
import sys
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

from models import WGS84Spheroid, EARTH_EQUATORIAL_RADIUS
from camera import Camera
from rendering import EarthRenderer


class TestWGS84Spheroid(unittest.TestCase):
    """Test the WGS84 Earth model."""
    
    def setUp(self):
        self.spheroid = WGS84Spheroid()
        self.test_position = np.array([-10000000.0, 0.0, 0.0])  # 10Mm from Earth
    
    def test_initialization(self):
        """Test spheroid initialization with correct parameters."""
        self.assertEqual(self.spheroid.equatorial_radius, EARTH_EQUATORIAL_RADIUS)
        self.assertGreater(self.spheroid.mean_radius, 6000000)  # Reasonable Earth radius
    
    def test_earth_disk_generation(self):
        """Test Earth disk point generation."""
        points, angular_radius = self.spheroid.get_earth_disk_points(self.test_position, num_points=100)
        
        # Check return types and shapes
        self.assertIsInstance(points, np.ndarray)
        self.assertIsInstance(angular_radius, float)
        self.assertEqual(points.shape[1], 3)  # 3D points
        self.assertGreater(len(points), 0)  # Some points generated
        self.assertGreater(angular_radius, 0)  # Positive angular radius
    
    def test_horizon_circle_generation(self):
        """Test horizon circle generation."""
        horizon_points = self.spheroid.get_earth_horizon_circle(self.test_position, num_points=36)
        
        # Check return type and shape
        self.assertIsInstance(horizon_points, np.ndarray)
        self.assertEqual(horizon_points.shape[1], 3)  # 3D points
        self.assertEqual(len(horizon_points), 36)  # Exact number requested
    
    def test_invalid_position(self):
        """Test that invalid camera positions raise errors."""
        inside_earth = np.array([1000000.0, 0.0, 0.0])  # Inside Earth
        
        with self.assertRaises(ValueError):
            self.spheroid.get_earth_disk_points(inside_earth)
        
        with self.assertRaises(ValueError):
            self.spheroid.get_earth_horizon_circle(inside_earth)


class TestCamera(unittest.TestCase):
    """Test the pinhole camera model."""
    
    def setUp(self):
        self.camera = Camera(
            position=[-10000000.0, 0.0, 0.0],
            orientation=[0.0, -90.0, 0.0],
            focal_length=35.0,
            pixel_size=0.01,
            image_width=1920,
            image_height=1080
        )
    
    def test_initialization(self):
        """Test camera initialization."""
        self.assertEqual(self.camera.image_width, 1920)
        self.assertEqual(self.camera.image_height, 1080)
        self.assertIsInstance(self.camera.rotation_matrix, np.ndarray)
        self.assertEqual(self.camera.rotation_matrix.shape, (3, 3))
    
    def test_coordinate_transformation(self):
        """Test world to camera coordinate transformation."""
        test_points = np.array([[0, 0, 0], [1000000, 0, 0], [0, 1000000, 0]])
        camera_points = self.camera.world_to_camera(test_points)
        
        self.assertIsInstance(camera_points, np.ndarray)
        self.assertEqual(camera_points.shape, test_points.shape)
    
    def test_image_projection(self):
        """Test 3D to 2D image projection."""
        # Test points in front of camera
        camera_points = np.array([[0, 0, 1000000], [100000, 0, 1000000]])
        projected, valid_mask = self.camera.project_to_image(camera_points)
        
        self.assertIsInstance(projected, np.ndarray)
        self.assertIsInstance(valid_mask, np.ndarray)
        self.assertEqual(projected.shape[1], 2)  # 2D image coordinates
        self.assertTrue(np.all(valid_mask))  # All points should be valid (in front)
    
    def test_field_of_view_calculation(self):
        """Test field of view calculations."""
        h_fov, v_fov = self.camera.get_field_of_view()
        d_fov = self.camera.get_diagonal_fov()
        
        self.assertGreater(h_fov, 0)
        self.assertGreater(v_fov, 0)
        self.assertGreater(d_fov, 0)
        self.assertGreater(d_fov, max(h_fov, v_fov))  # Diagonal should be largest


class TestEarthRenderer(unittest.TestCase):
    """Test the Earth rendering system."""
    
    def setUp(self):
        self.camera = Camera(
            position=[-10000000.0, 0.0, 0.0],
            orientation=[0.0, -90.0, 0.0],
            focal_length=5.0,  # Wide field of view
            pixel_size=0.1,
            image_width=640,   # Smaller for faster tests
            image_height=480
        )
        self.spheroid = WGS84Spheroid()
        self.renderer = EarthRenderer(self.camera, self.spheroid)
    
    def test_image_generation(self):
        """Test complete image generation pipeline."""
        with tempfile.NamedTemporaryFile(suffix='.png', delete=False) as tmp_file:
            try:
                # Generate image
                stats = self.renderer.render_image(tmp_file.name)
                
                # Check that image file was created
                self.assertTrue(os.path.exists(tmp_file.name))
                self.assertGreater(os.path.getsize(tmp_file.name), 1000)  # Non-empty file
                
                # Check statistics
                self.assertIsInstance(stats, dict)
                required_keys = ['earth_fill_percentage', 'angular_radius_degrees', 
                               'horizon_captured', 'image_width', 'image_height']
                for key in required_keys:
                    self.assertIn(key, stats)
                
                # Validate statistics ranges
                self.assertGreaterEqual(stats['earth_fill_percentage'], 0)
                self.assertLessEqual(stats['earth_fill_percentage'], 100)
                self.assertGreater(stats['angular_radius_degrees'], 0)
                self.assertEqual(stats['image_width'], 640)
                self.assertEqual(stats['image_height'], 480)
                
            finally:
                # Clean up
                if os.path.exists(tmp_file.name):
                    os.unlink(tmp_file.name)


class TestIntegration(unittest.TestCase):
    """Integration tests for the complete system."""
    
    def test_different_camera_positions(self):
        """Test rendering from different orbital positions."""
        spheroid = WGS84Spheroid()
        positions = [
            [-8000000.0, 0.0, 0.0],   # Close to Earth
            [-15000000.0, 0.0, 0.0],  # Medium distance
            [-25000000.0, 0.0, 0.0]   # Far from Earth
        ]
        
        for position in positions:
            with self.subTest(position=position):
                camera = Camera(
                    position=position,
                    orientation=[0.0, -90.0, 0.0],
                    focal_length=10.0,
                    pixel_size=0.05,
                    image_width=320,
                    image_height=240
                )
                
                renderer = EarthRenderer(camera, spheroid)
                
                with tempfile.NamedTemporaryFile(suffix='.png', delete=False) as tmp_file:
                    try:
                        stats = renderer.render_image(tmp_file.name)
                        
                        # Verify reasonable results
                        self.assertTrue(os.path.exists(tmp_file.name))
                        self.assertGreater(stats['angular_radius_degrees'], 0)
                        
                        # Further positions should have smaller angular sizes
                        distance = np.linalg.norm(position)
                        expected_angular_size = np.degrees(np.arcsin(spheroid.mean_radius / distance))
                        self.assertAlmostEqual(stats['angular_radius_degrees'], 
                                             expected_angular_size, places=2)
                    
                    finally:
                        if os.path.exists(tmp_file.name):
                            os.unlink(tmp_file.name)


if __name__ == '__main__':
    unittest.main()
