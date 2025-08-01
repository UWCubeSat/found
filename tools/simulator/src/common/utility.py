
"""
Utility functions for satellite imaging simulations.

This module provides utility functions for calculating camera parameters,
field of view, and other imaging-related calculations.
"""

import numpy as np
from typing import List
from .constants import LEO_DISTANCES, SIMULATION_RANGES, EARTH_MEAN_RADIUS

# ==============================================================================
# CAMERA PARAMETER CALCULATIONS
# ==============================================================================

def calculate_focal_length_from_fov_and_resolution(half_fov_radians: float, 
                                                   resolution_pixels: int, 
                                                   pixel_size_meters: float) -> float:
    """
    Calculate focal length from field of view, resolution, and pixel size.
    Based on the template: focal_length = (resolution * pixel_size) / 2 / tan(half_fov)
    
    Args:
        half_fov_radians: Half field of view in radians
        resolution_pixels: Sensor resolution in pixels (square sensor)
        pixel_size_meters: Physical pixel size in meters
        
    Returns:
        Focal length in meters
    """
    return (resolution_pixels * pixel_size_meters) / 2 / np.tan(half_fov_radians)

def calculate_focal_length_from_fov(fov_degrees: float, sensor_width_mm: float) -> float:
    """
    Calculate focal length needed to achieve a specific horizontal FOV.
    
    Args:
        fov_degrees: Desired horizontal field of view in degrees
        sensor_width_mm: Sensor width in millimeters
        
    Returns:
        Required focal length in millimeters
    """
    fov_radians = np.radians(fov_degrees)
    return sensor_width_mm / (2 * np.tan(fov_radians / 2))

def calculate_fov(sensor_size_mm: float, focal_length_mm: float) -> float:
    """
    Calculate field of view in radians.
    
    Args:
        sensor_size_mm: Sensor dimension in millimeters
        focal_length_mm: Focal length in millimeters
        
    Returns:
        Field of view in radians
    """
    return 2 * np.arctan(sensor_size_mm / (2 * focal_length_mm))

def calculate_pixel_size_for_square_sensor(sensor_width_mm: float, resolution_pixels: int) -> float:
    """
    Calculate pixel size for a square sensor.
    
    Args:
        sensor_width_mm: Physical sensor width in millimeters
        resolution_pixels: Number of pixels along one side (assuming square)
        
    Returns:
        Pixel size in micrometers
    """
    pixel_size_mm = sensor_width_mm / resolution_pixels
    return pixel_size_mm * 1000  # Convert to micrometers

# ==============================================================================
# SIMULATION PARAMETER ACCESS
# ==============================================================================

def get_leo_distance(altitude_name: str) -> float:
    """Get LEO distance by altitude name."""
    if altitude_name not in LEO_DISTANCES:
        available = ', '.join(LEO_DISTANCES.keys())
        raise ValueError(f"Unknown altitude '{altitude_name}'. Available: {available}")
    return LEO_DISTANCES[altitude_name]

def get_fov_range(fov_category: str) -> np.ndarray:
    """Get FOV range by category name."""
    if fov_category not in SIMULATION_RANGES['fovs']:
        available = ', '.join(SIMULATION_RANGES['fovs'].keys())
        raise ValueError(f"Unknown FOV category '{fov_category}'. Available: {available}")
    return SIMULATION_RANGES['fovs'][fov_category]

def get_resolution_range(resolution_category: str) -> List[int]:
    """Get resolution range by category name."""
    if resolution_category not in SIMULATION_RANGES['resolutions']:
        available = ', '.join(SIMULATION_RANGES['resolutions'].keys())
        raise ValueError(f"Unknown resolution category '{resolution_category}'. Available: {available}")
    return SIMULATION_RANGES['resolutions'][resolution_category]