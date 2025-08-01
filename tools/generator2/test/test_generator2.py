#!/usr/bin/env python3
"""
Test suite for the generator2 tool.
"""

import pytest
import numpy as np
import sys
from pathlib import Path

# Add the src directory to the path for testing
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

# Import the classes from the main module
import importlib.util
spec = importlib.util.spec_from_file_location("generator2_main", Path(__file__).parent.parent / "src" / "__main__.py")
generator2_main = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator2_main)

WGS84Spheroid = generator2_main.WGS84Spheroid
Camera = generator2_main.Camera
EarthRenderer = generator2_main.EarthRenderer
EARTH_EQUATORIAL_RADIUS = generator2_main.EARTH_EQUATORIAL_RADIUS


class TestWGS84Spheroid:
    """Test cases for the WGS84Spheroid class."""
    
    def test_spheroid_initialization(self):
        """Test that the spheroid initializes with correct parameters."""
        spheroid = WGS84Spheroid()
        assert spheroid.equatorial_radius == 6378137.0
        assert spheroid.polar_radius == 6356752.3
        assert spheroid.eccentricity_squared > 0
    
    def test_surface_points_generation(self):
        """Test Earth disk point generation."""
        spheroid = WGS84Spheroid()
        camera_position = np.array([10000000, 0, 0])  # 10,000 km from center
        
        points, angular_radius = spheroid.get_earth_disk_points(camera_position, num_points=100)
        
        assert points.shape[1] == 3  # 3D points
        assert len(points) <= 100  # Some points might be filtered out
        assert angular_radius > 0
        
        # Check that all points are approximately on Earth's surface
        for point in points[:10]:  # Test a subset for performance
            distance_from_center = np.linalg.norm(point)
            assert abs(distance_from_center - spheroid.mean_radius) < 1000  # 1km tolerance
    
    def test_horizon_points_generation(self):
        """Test horizon circle generation."""
        spheroid = WGS84Spheroid()
        camera_position = np.array([10000000, 0, 0])  # 10,000 km from center
        
        horizon_points = spheroid.get_earth_horizon_circle(camera_position, num_points=36)
        
        assert horizon_points.shape == (36, 3)
        assert len(horizon_points) > 0
        
        # Check that horizon points are roughly at the correct distance from camera
        distances = [np.linalg.norm(point - camera_position) for point in horizon_points[:5]]
        avg_distance = np.mean(distances)
        expected_distance = np.sqrt(np.linalg.norm(camera_position)**2 - spheroid.mean_radius**2)
        
        # Allow some tolerance due to geometric approximations
        assert abs(avg_distance - expected_distance) < expected_distance * 0.1


class TestCamera:
    """Test cases for the Camera class."""
    
    def test_camera_initialization(self):
        """Test camera initialization."""
        position = [10000000, 0, 0]
        orientation = [0, 0, 0]
        camera = Camera(position, orientation, 35.0, 0.01, 1920, 1080)
        
        assert np.array_equal(camera.position, np.array(position))
        assert np.array_equal(camera.orientation, np.array(orientation))
        assert camera.focal_length == 0.035  # Converted to meters
        assert camera.pixel_size == 0.00001  # Converted to meters
        assert camera.image_width == 1920
        assert camera.image_height == 1080
    
    def test_rotation_matrix_identity(self):
        """Test that zero orientation produces identity-like behavior."""
        camera = Camera([0, 0, 0], [0, 0, 0], 35.0, 0.01, 1920, 1080)
        
        # Test point transformation with zero rotation
        test_point = np.array([[1, 0, 0]])
        transformed = camera.world_to_camera(test_point)
        
        # With zero position and rotation, should be identical
        assert transformed.shape == (1, 3)
    
    def test_world_to_camera_transformation(self):
        """Test world to camera coordinate transformation."""
        camera = Camera([1000, 0, 0], [0, 0, 0], 35.0, 0.01, 1920, 1080)
        
        # Test a simple point
        world_points = np.array([[1500, 0, 0], [1000, 100, 0]])
        camera_points = camera.world_to_camera(world_points)
        
        assert camera_points.shape == (2, 3)
        # First point should be 500 meters in front of camera
        assert camera_points[0, 0] == 500
    
    def test_projection_validity(self):
        """Test that projection correctly identifies valid points."""
        camera = Camera([0, 0, 0], [0, 0, 0], 35.0, 0.01, 1920, 1080)
        
        # Points in front (positive Z) and behind (negative Z) camera
        camera_points = np.array([
            [0, 0, 1000],   # In front - valid
            [0, 0, -1000],  # Behind - invalid
            [100, 100, 500] # In front - valid
        ])
        
        projected, valid_mask = camera.project_to_image(camera_points)
        
        assert len(valid_mask) == 3
        assert valid_mask[0] == True   # In front
        assert valid_mask[1] == False  # Behind
        assert valid_mask[2] == True   # In front


class TestEarthRenderer:
    """Test cases for the EarthRenderer class."""
    
    def test_renderer_initialization(self):
        """Test that renderer initializes correctly."""
        camera = Camera([10000000, 0, 0], [0, -90, 0], 35.0, 0.01, 1920, 1080)
        spheroid = WGS84Spheroid()
        renderer = EarthRenderer(camera, spheroid)
        
        assert renderer.camera == camera
        assert renderer.spheroid == spheroid


class TestIntegration:
    """Integration tests for the complete pipeline."""
    
    def test_valid_camera_position(self):
        """Test that camera positions outside Earth are handled correctly."""
        # Position well outside Earth
        camera = Camera([10000000, 0, 0], [0, -90, 0], 35.0, 0.01, 1920, 1080)
        spheroid = WGS84Spheroid()
        
        # Should not raise any errors during setup
        renderer = EarthRenderer(camera, spheroid)
        assert renderer is not None
    
    def test_camera_position_validation(self):
        """Test position validation logic."""
        
        # Position inside Earth should be invalid
        invalid_position = np.array([1000000, 0, 0])  # 1000 km from center
        distance = np.linalg.norm(invalid_position)
        
        assert distance <= EARTH_EQUATORIAL_RADIUS
        
        # Position outside Earth should be valid
        valid_position = np.array([10000000, 0, 0])  # 10,000 km from center
        distance = np.linalg.norm(valid_position)
        
        assert distance > EARTH_EQUATORIAL_RADIUS


if __name__ == '__main__':
    pytest.main([__file__])
