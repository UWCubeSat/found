#!/usr/bin/env python3
"""
Command-line interface tests for the generator2 tool.
Tests the CLI behavior, argument parsing, and error handling.
"""

import pytest
import subprocess
import tempfile
import os
import json
from pathlib import Path
import sys

# Add the parent directory to test the CLI
TOOL_PATH = Path(__file__).parent.parent
FOUND_ROOT = TOOL_PATH.parent.parent


class TestCLIBehavior:
    """Test command-line interface behavior."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
    
    def teardown_method(self):
        """Clean up test files."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def run_generator2(self, args, expect_success=True):
        """Helper to run the generator2 tool with given arguments."""
        cmd = [sys.executable, "-m", "tools.generator2"] + args
        result = subprocess.run(cmd, cwd=FOUND_ROOT, capture_output=True, text=True)
        
        if expect_success:
            assert result.returncode == 0, f"Command failed: {result.stderr}"
        
        return result
    
    def test_help_command(self):
        """Test that help command works."""
        result = self.run_generator2(["--help"])
        
        assert "Generate synthetic 2D images of Earth" in result.stdout
        assert "--position" in result.stdout
        assert "--orientation" in result.stdout
        assert "--focal-length" in result.stdout
    
    def test_basic_valid_command(self):
        """Test a basic valid command."""
        output_file = os.path.join(self.temp_dir, "test_output.png")
        
        result = self.run_generator2([
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "-90", "0",
            "--output", output_file
        ])
        
        # Should complete successfully
        assert result.returncode == 0
        assert os.path.exists(output_file)
        
        # Should contain statistics in output
        assert "Earth fills" in result.stdout
        assert "Horizon captured" in result.stdout
        assert "Image resolution" in result.stdout
    
    def test_position_inside_earth_error(self):
        """Test error when camera position is inside Earth."""
        output_file = os.path.join(self.temp_dir, "invalid.png")
        
        result = self.run_generator2([
            "--position", "1000000", "0", "0",  # Inside Earth
            "--orientation", "0", "-90", "0",
            "--output", output_file
        ], expect_success=False)
        
        assert result.returncode != 0
        assert "inside Earth" in result.stderr
        assert not os.path.exists(output_file)
    
    def test_custom_camera_parameters(self):
        """Test custom camera parameters."""
        output_file = os.path.join(self.temp_dir, "custom_params.png")
        
        result = self.run_generator2([
            "--position", "-8000000", "2000000", "1000000",
            "--orientation", "30", "-45", "15",
            "--focal-length", "25.0",
            "--pixel-size", "0.015",
            "--image-size", "1280", "720",
            "--output", output_file
        ])
        
        assert result.returncode == 0
        assert os.path.exists(output_file)
        assert "1280×720" in result.stdout
    
    def test_auto_generated_filename(self):
        """Test auto-generated filename when no output specified."""
        result = self.run_generator2([
            "--position", "-7000000", "0", "0",
            "--orientation", "0", "-90", "0"
        ])
        
        assert result.returncode == 0
        
        # Should generate a filename based on position and orientation
        expected_pattern = "(-7000000.00, 0.00, 0.00),(0.00, -90.00, 0.00).png"
        assert os.path.exists(os.path.join(FOUND_ROOT, expected_pattern))
        
        # Clean up
        try:
            os.remove(os.path.join(FOUND_ROOT, expected_pattern))
        except FileNotFoundError:
            pass
    
    def test_verbose_output(self):
        """Test verbose output flag."""
        output_file = os.path.join(self.temp_dir, "verbose_test.png")
        
        result = self.run_generator2([
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "-90", "0",
            "--output", output_file,
            "--verbose"
        ])
        
        assert result.returncode == 0
        
        # Verbose mode should include debug information
        assert "Initializing camera and Earth models" in result.stderr
        assert "Generating Earth disk points" in result.stderr
    
    def test_missing_required_arguments(self):
        """Test error when required arguments are missing."""
        # Missing position
        result = self.run_generator2([
            "--orientation", "0", "-90", "0"
        ], expect_success=False)
        assert result.returncode != 0
        
        # Missing orientation
        result = self.run_generator2([
            "--position", "-10000000", "0", "0"
        ], expect_success=False)
        assert result.returncode != 0
    
    def test_invalid_argument_values(self):
        """Test error handling for invalid argument values."""
        output_file = os.path.join(self.temp_dir, "invalid_args.png")
        
        # Invalid focal length (negative)
        result = self.run_generator2([
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "-90", "0",
            "--focal-length", "-10",
            "--output", output_file
        ], expect_success=False)
        assert result.returncode != 0
        
        # Invalid pixel size (negative)
        result = self.run_generator2([
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "-90", "0",
            "--pixel-size", "-0.01",
            "--output", output_file
        ], expect_success=False)
        assert result.returncode != 0
        
        # Invalid image size (zero)
        result = self.run_generator2([
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "-90", "0",
            "--image-size", "0", "100",
            "--output", output_file
        ], expect_success=False)
        assert result.returncode != 0


class TestPositionOrientationBehavior:
    """Test that position and orientation parameters work as expected."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
    
    def teardown_method(self):
        """Clean up test files."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def run_generator2_and_get_stats(self, position, orientation, **kwargs):
        """Helper to run generator2 and extract statistics from output."""
        output_file = os.path.join(self.temp_dir, f"test_{len(os.listdir(self.temp_dir))}.png")
        
        args = [
            "--position", str(position[0]), str(position[1]), str(position[2]),
            "--orientation", str(orientation[0]), str(orientation[1]), str(orientation[2]),
            "--output", output_file
        ]
        
        # Add any additional arguments
        for key, value in kwargs.items():
            key_arg = f"--{key.replace('_', '-')}"
            if isinstance(value, list):
                args.extend([key_arg] + [str(v) for v in value])
            else:
                args.extend([key_arg, str(value)])
        
        cmd = [sys.executable, "-m", "tools.generator2"] + args
        result = subprocess.run(cmd, cwd=FOUND_ROOT, capture_output=True, text=True)
        
        assert result.returncode == 0, f"Command failed: {result.stderr}"
        assert os.path.exists(output_file)
        
        # Parse statistics from output
        stats = {}
        for line in result.stdout.split('\n'):
            if "Earth fills" in line:
                # Extract percentage
                import re
                match = re.search(r'(\d+\.\d+)%', line)
                if match:
                    stats['earth_fill_percentage'] = float(match.group(1))
            elif "Earth's angular size" in line:
                match = re.search(r'(\d+\.\d+)°', line)
                if match:
                    stats['angular_size'] = float(match.group(1))
            elif "Horizon captured" in line:
                stats['horizon_captured'] = '✓' in line
        
        return stats, output_file
    
    def test_distance_affects_earth_size(self):
        """Test that distance from Earth affects Earth's apparent size."""
        orientation = [0, -90, 0]  # Looking directly at Earth
        
        # Test different distances
        distances = [7000000, 15000000, 25000000]  # Close, medium, far
        angular_sizes = []
        
        for distance in distances:
            position = [distance, 0, 0]
            stats, _ = self.run_generator2_and_get_stats(
                position, orientation, 
                focal_length=15, pixel_size=0.03
            )
            
            assert 'angular_size' in stats
            angular_sizes.append(stats['angular_size'])
        
        # Angular size should decrease with distance
        assert angular_sizes[0] > angular_sizes[1] > angular_sizes[2]
        print(f"Angular sizes at different distances: {angular_sizes}")
    
    def test_orientation_affects_view(self):
        """Test that different orientations produce different views."""
        position = [10000000, 0, 0]
        
        # Test different orientations
        orientations = [
            [0, -90, 0],   # Looking directly at Earth
            [90, -90, 0],  # Rotated 90 degrees
            [0, -45, 0],   # Different pitch angle
            [0, -90, 45]   # Different roll angle
        ]
        
        results = []
        for i, orientation in enumerate(orientations):
            stats, output_file = self.run_generator2_and_get_stats(
                position, orientation,
                focal_length=20, pixel_size=0.02
            )
            results.append((orientation, stats, output_file))
        
        # Each orientation should produce a valid result
        for orientation, stats, output_file in results:
            assert os.path.exists(output_file)
            if 'earth_fill_percentage' in stats:
                assert stats['earth_fill_percentage'] > 0
        
        print(f"Tested {len(orientations)} different orientations successfully")
    
    def test_field_of_view_affects_horizon_capture(self):
        """Test that field of view affects horizon capture."""
        position = [8000000, 0, 0]
        orientation = [0, -90, 0]
        
        # Test different fields of view
        fov_configs = [
            {"focal_length": 50, "pixel_size": 0.01},  # Narrow FOV
            {"focal_length": 25, "pixel_size": 0.02},  # Medium FOV
            {"focal_length": 10, "pixel_size": 0.05}   # Wide FOV
        ]
        
        horizon_captures = []
        for config in fov_configs:
            stats, _ = self.run_generator2_and_get_stats(
                position, orientation, **config
            )
            horizon_captures.append(stats.get('horizon_captured', False))
        
        # Wide FOV should be more likely to capture horizon
        print(f"Horizon capture with different FOVs: {horizon_captures}")
        # At least one configuration should capture the horizon
        assert any(horizon_captures), "At least one FOV configuration should capture the horizon"
    
    def test_position_coordinates_consistency(self):
        """Test that position coordinates work consistently."""
        orientation = [0, -90, 0]
        distance = 10000000
        
        # Test positions at same distance but different coordinates
        positions = [
            [distance, 0, 0],        # X-axis
            [0, distance, 0],        # Y-axis
            [0, 0, distance],        # Z-axis
            [distance/1.414, distance/1.414, 0]  # 45-degree angle in XY plane
        ]
        
        angular_sizes = []
        for position in positions:
            stats, _ = self.run_generator2_and_get_stats(
                position, orientation,
                focal_length=25, pixel_size=0.02
            )
            
            if 'angular_size' in stats:
                angular_sizes.append(stats['angular_size'])
        
        # All positions at same distance should give similar angular sizes
        if len(angular_sizes) > 1:
            avg_size = sum(angular_sizes) / len(angular_sizes)
            for size in angular_sizes:
                assert abs(size - avg_size) < avg_size * 0.1  # Within 10%
        
        print(f"Angular sizes from same distance: {angular_sizes}")


class TestEdgeCases:
    """Test edge cases and error conditions."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
    
    def teardown_method(self):
        """Clean up test files."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_camera_pointing_away_from_earth(self):
        """Test camera pointing directly away from Earth."""
        output_file = os.path.join(self.temp_dir, "pointing_away.png")
        
        result = subprocess.run([
            sys.executable, "-m", "tools.generator2",
            "--position", "-10000000", "0", "0",
            "--orientation", "0", "90", "0",  # Looking away from Earth
            "--output", output_file
        ], cwd=FOUND_ROOT, capture_output=True, text=True)
        
        # Should fail because Earth is not visible
        assert result.returncode != 0
        assert "not visible" in result.stderr
    
    def test_extreme_distances(self):
        """Test with extreme distances from Earth."""
        # Very close (but still outside Earth)
        close_distance = 6400000  # Just above Earth's surface
        output_file = os.path.join(self.temp_dir, "very_close.png")
        
        result = subprocess.run([
            sys.executable, "-m", "tools.generator2",
            "--position", str(close_distance), "0", "0",
            "--orientation", "0", "-90", "0",
            "--focal-length", "5",  # Very wide FOV needed
            "--pixel-size", "0.1",
            "--output", output_file
        ], cwd=FOUND_ROOT, capture_output=True, text=True)
        
        # Should work with appropriate FOV
        assert result.returncode == 0
        assert os.path.exists(output_file)
        
        # Very far distance
        far_distance = 100000000  # 100,000 km
        output_file2 = os.path.join(self.temp_dir, "very_far.png")
        
        result = subprocess.run([
            sys.executable, "-m", "tools.generator2",
            "--position", str(far_distance), "0", "0",
            "--orientation", "0", "-90", "0",
            "--focal-length", "100",  # Narrow FOV to see small Earth
            "--pixel-size", "0.001",
            "--output", output_file2
        ], cwd=FOUND_ROOT, capture_output=True, text=True)
        
        assert result.returncode == 0
        assert os.path.exists(output_file2)
    
    def test_extreme_orientations(self):
        """Test with extreme orientation angles."""
        position = [10000000, 0, 0]
        output_file = os.path.join(self.temp_dir, "extreme_orientation.png")
        
        # Test with large angles
        result = subprocess.run([
            sys.executable, "-m", "tools.generator2",
            "--position", str(position[0]), str(position[1]), str(position[2]),
            "--orientation", "720", "-450", "180",  # Large angles (should be normalized)
            "--output", output_file
        ], cwd=FOUND_ROOT, capture_output=True, text=True)
        
        # Should handle angle normalization
        assert result.returncode == 0
        assert os.path.exists(output_file)


if __name__ == '__main__':
    pytest.main([__file__, '-v'])
