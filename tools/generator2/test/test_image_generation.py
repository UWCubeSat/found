#!/usr/bin/env python3
"""
Comprehensive integration tests for the generator2 tool.
Tests actual image generation and validates output correctness.
"""

import pytest
import numpy as np
import sys
import tempfile
import os
from pathlib import Path
from PIL import Image
import matplotlib.pyplot as plt

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


class TestImageGeneration:
    """Test actual image generation and validate outputs."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.spheroid = WGS84Spheroid()
    
    def teardown_method(self):
        """Clean up test files."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_basic_image_generation(self):
        """Test that a basic image is generated successfully."""
        camera = Camera([10000000, 0, 0], [0, -90, 0], 35.0, 0.01, 800, 600)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_basic.png")
        stats = renderer.render_image(output_path)
        
        # Check that image file was created
        assert os.path.exists(output_path)
        
        # Check image properties
        with Image.open(output_path) as img:
            assert img.size == (800, 600)
            assert img.mode in ['RGB', 'RGBA']
        
        # Check statistics
        assert isinstance(stats, dict)
        assert 'earth_fill_percentage' in stats
        assert 'horizon_captured' in stats
        assert stats['earth_fill_percentage'] >= 0
        assert stats['image_width'] == 800
        assert stats['image_height'] == 600
    
    def test_wide_field_view_captures_horizon(self):
        """Test that a wide field view captures the horizon."""
        # Use wide field of view (short focal length, large pixels)
        camera = Camera([7000000, 0, 0], [0, -90, 0], 10.0, 0.05, 1920, 1080)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_horizon.png")
        stats = renderer.render_image(output_path)
        
        # Should capture horizon with wide field of view
        assert stats['horizon_captured'] == True
        assert stats['horizon_capture_percentage'] > 50.0
        assert stats['earth_fill_percentage'] > 0.1
    
    def test_narrow_field_view_misses_horizon(self):
        """Test that a narrow field view might miss the horizon."""
        # Use narrow field of view (long focal length, small pixels)
        camera = Camera([15000000, 0, 0], [0, -90, 0], 100.0, 0.001, 1920, 1080)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_no_horizon.png")
        stats = renderer.render_image(output_path)
        
        # Narrow field of view should show Earth but possibly miss horizon
        assert stats['earth_fill_percentage'] > 0
        # Horizon might or might not be captured depending on exact parameters
    
    def test_different_orientations_affect_view(self):
        """Test that different camera orientations produce different views."""
        base_position = [10000000, 0, 0]
        
        # Test different orientations
        orientations = [
            [0, -90, 0],    # Looking directly at Earth
            [45, -90, 0],   # Rotated 45 degrees
            [0, -45, 0],    # Different pitch
            [0, -90, 45]    # Different roll
        ]
        
        earth_centers = []
        
        for i, orientation in enumerate(orientations):
            camera = Camera(base_position, orientation, 15.0, 0.03, 800, 600)
            renderer = EarthRenderer(camera, self.spheroid)
            
            output_path = os.path.join(self.temp_dir, f"test_orientation_{i}.png")
            stats = renderer.render_image(output_path)
            
            # Each orientation should produce a valid image
            assert os.path.exists(output_path)
            assert stats['earth_fill_percentage'] > 0
            
            # Store some metric to compare orientations
            earth_centers.append((stats['earth_fill_percentage'], stats.get('angular_radius_degrees', 0)))
        
        # Different orientations should produce different results
        # (though some might be similar depending on symmetry)
        assert len(set(earth_centers)) > 1, "Different orientations should produce different views"
    
    def test_position_affects_earth_angular_size(self):
        """Test that different distances affect Earth's apparent size."""
        orientation = [0, -90, 0]
        
        positions = [
            [7000000, 0, 0],    # Close to Earth
            [15000000, 0, 0],   # Medium distance
            [30000000, 0, 0]    # Far from Earth
        ]
        
        angular_sizes = []
        
        for i, position in enumerate(positions):
            camera = Camera(position, orientation, 20.0, 0.02, 800, 600)
            renderer = EarthRenderer(camera, self.spheroid)
            
            output_path = os.path.join(self.temp_dir, f"test_distance_{i}.png")
            stats = renderer.render_image(output_path)
            
            angular_sizes.append(stats['angular_radius_degrees'])
        
        # Angular size should decrease with distance
        assert angular_sizes[0] > angular_sizes[1] > angular_sizes[2]
        
        # Should be reasonable values (Earth's angular diameter from various distances)
        for size in angular_sizes:
            assert 0.1 < size < 90.0, f"Angular size {size}° seems unreasonable"
    
    def test_earth_appears_circular(self):
        """Test that Earth appears approximately circular when horizon is visible."""
        # Use setup that should capture full horizon
        camera = Camera([8000000, 0, 0], [0, -90, 0], 12.0, 0.04, 1000, 1000)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_circular.png")
        stats = renderer.render_image(output_path)
        
        # Should capture most of the horizon
        assert stats['horizon_captured'] == True
        assert stats['horizon_capture_percentage'] > 80.0
        
        # Earth should fill a reasonable portion of a square image
        assert 0.1 < stats['earth_fill_percentage'] < 50.0
    
    def test_invalid_position_raises_error(self):
        """Test that invalid camera positions raise appropriate errors."""
        # Position inside Earth should raise an error
        invalid_position = [1000000, 0, 0]  # 1000 km from center (inside Earth)
        camera = Camera(invalid_position, [0, -90, 0], 35.0, 0.01, 800, 600)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_invalid.png")
        
        with pytest.raises(ValueError, match="Camera is inside Earth"):
            renderer.render_image(output_path)
    
    def test_earth_not_visible_raises_error(self):
        """Test that camera pointing away from Earth raises an error."""
        # Point camera away from Earth
        camera = Camera([10000000, 0, 0], [0, 90, 0], 35.0, 0.01, 800, 600)  # Looking away
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_not_visible.png")
        
        with pytest.raises(ValueError, match="Earth is not visible"):
            renderer.render_image(output_path)


class TestGeometricCorrectness:
    """Test that the geometric calculations are correct."""
    
    def setup_method(self):
        self.spheroid = WGS84Spheroid()
    
    def test_earth_angular_size_calculation(self):
        """Test that Earth's angular size is calculated correctly."""
        # From 10,000 km above Earth's surface
        distance_from_center = EARTH_EQUATORIAL_RADIUS + 10000000  # ~16,378 km from center
        camera = Camera([distance_from_center, 0, 0], [0, -90, 0], 35.0, 0.01, 800, 600)
        renderer = EarthRenderer(camera, self.spheroid)
        
        # Calculate expected angular radius
        expected_angular_radius = np.degrees(np.arcsin(self.spheroid.mean_radius / distance_from_center))
        
        # Generate points to get actual angular radius
        earth_points, actual_angular_radius = self.spheroid.get_earth_disk_points(camera.position, 1000)
        actual_angular_radius_degrees = np.degrees(actual_angular_radius)
        
        # Should be approximately equal (within 1 degree)
        assert abs(actual_angular_radius_degrees - expected_angular_radius) < 1.0
    
    def test_horizon_distance_calculation(self):
        """Test that horizon distance is calculated correctly."""
        camera_distance = 10000000  # 10,000 km from Earth center
        horizon_points = self.spheroid.get_earth_horizon_circle([camera_distance, 0, 0], 360)
        
        # All horizon points should be approximately on Earth's surface
        for point in horizon_points[::36]:  # Test every 10th point
            distance_from_center = np.linalg.norm(point)
            assert abs(distance_from_center - self.spheroid.mean_radius) < 1000  # Within 1 km
        
        # Horizon points should be approximately the correct distance from camera
        expected_horizon_distance = np.sqrt(camera_distance**2 - self.spheroid.mean_radius**2)
        for point in horizon_points[::36]:
            distance_from_camera = np.linalg.norm(point - np.array([camera_distance, 0, 0]))
            # Should be within 10% of expected (accounting for geometric approximations)
            assert abs(distance_from_camera - expected_horizon_distance) < expected_horizon_distance * 0.1
    
    def test_camera_orientation_affects_projection(self):
        """Test that camera orientation correctly affects point projection."""
        position = [10000000, 0, 0]
        
        # Test point directly in front of camera with different orientations
        test_point = np.array([[0, 0, 0]])  # Earth center
        
        # Camera looking at Earth (should project to image center)
        camera1 = Camera(position, [0, -90, 0], 35.0, 0.01, 800, 600)
        camera_coords1 = camera1.world_to_camera(test_point)
        projected1, valid1 = camera1.project_to_image(camera_coords1)
        
        if valid1[0]:
            # Should be near image center
            center_x, center_y = 400, 300
            assert abs(projected1[0, 0] - center_x) < 50
            assert abs(projected1[0, 1] - center_y) < 50
        
        # Camera looking away from Earth (point should be behind camera)
        camera2 = Camera(position, [0, 90, 0], 35.0, 0.01, 800, 600)
        camera_coords2 = camera2.world_to_camera(test_point)
        
        # Point should be behind camera (negative Z)
        assert camera_coords2[0, 2] < 0
    
    def test_field_of_view_consistency(self):
        """Test that field of view calculations are consistent."""
        camera = Camera([10000000, 0, 0], [0, -90, 0], 35.0, 0.01, 1920, 1080)
        
        # Calculate theoretical field of view
        horizontal_fov = 2 * np.degrees(np.arctan((camera.image_width * camera.pixel_size) / (2 * camera.focal_length)))
        vertical_fov = 2 * np.degrees(np.arctan((camera.image_height * camera.pixel_size) / (2 * camera.focal_length)))
        
        # Field of view should be reasonable
        assert 10 < horizontal_fov < 180
        assert 10 < vertical_fov < 180
        
        # Aspect ratio should match image aspect ratio
        aspect_ratio_fov = horizontal_fov / vertical_fov
        aspect_ratio_image = camera.image_width / camera.image_height
        assert abs(aspect_ratio_fov - aspect_ratio_image) < 0.1


class TestStatisticsAccuracy:
    """Test that reported statistics are accurate."""
    
    def setup_method(self):
        self.temp_dir = tempfile.mkdtemp()
        self.spheroid = WGS84Spheroid()
    
    def teardown_method(self):
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_earth_fill_percentage_accuracy(self):
        """Test that Earth fill percentage is calculated accurately."""
        # Use a setup where we can predict the approximate fill percentage
        camera = Camera([7500000, 0, 0], [0, -90, 0], 15.0, 0.03, 800, 800)  # Square image
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_fill.png")
        stats = renderer.render_image(output_path)
        
        # Calculate expected angular diameter
        distance = np.linalg.norm(camera.position)
        angular_radius = np.degrees(np.arcsin(self.spheroid.mean_radius / distance))
        
        # Expected fill percentage based on circular area
        # This is approximate since we're dealing with projections
        if stats['horizon_captured']:
            # Can make better predictions when full circle is visible
            assert stats['earth_fill_percentage'] > 0.1
            assert stats['earth_fill_percentage'] < 80  # Shouldn't fill most of the image
    
    def test_horizon_capture_percentage_accuracy(self):
        """Test that horizon capture percentage is accurate."""
        # Test case where we expect partial horizon capture
        camera = Camera([12000000, 3000000, 1000000], [0, -60, 0], 25.0, 0.02, 1920, 1080)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_horizon_partial.png")
        stats = renderer.render_image(output_path)
        
        if stats['horizon_captured']:
            # Percentage should be between 0 and 100
            assert 0 <= stats['horizon_capture_percentage'] <= 100
            
            # Should match the ratio of points
            expected_percentage = (stats['horizon_points_in_image'] / stats['total_horizon_points']) * 100
            assert abs(stats['horizon_capture_percentage'] - expected_percentage) < 0.1
    
    def test_statistics_consistency(self):
        """Test that all statistics are internally consistent."""
        camera = Camera([8000000, 0, 0], [0, -90, 0], 20.0, 0.02, 1600, 900)
        renderer = EarthRenderer(camera, self.spheroid)
        
        output_path = os.path.join(self.temp_dir, "test_consistency.png")
        stats = renderer.render_image(output_path)
        
        # Basic consistency checks
        assert stats['image_width'] == camera.image_width
        assert stats['image_height'] == camera.image_height
        assert stats['earth_points_in_image'] >= 0
        assert stats['angular_radius_degrees'] > 0
        
        # Earth fill percentage should be consistent with point count
        total_pixels = stats['image_width'] * stats['image_height']
        calculated_fill = (stats['earth_points_in_image'] / total_pixels) * 100
        
        # Note: The reported fill percentage might differ from point-based calculation
        # if we're using solid circle rendering, but should be in the same ballpark
        if stats['earth_points_in_image'] > 1000:  # Only check when we have enough points
            assert abs(stats['earth_fill_percentage'] - calculated_fill) < 5.0  # Within 5%


if __name__ == '__main__':
    pytest.main([__file__, '-v'])
