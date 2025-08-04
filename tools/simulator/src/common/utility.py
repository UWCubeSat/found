
"""
Utility functions for satellite imaging simulations.

This module provides utility functions for calculating camera parameters,
field of view, and other imaging-related calculations.
"""

import numpy as np
from typing import List


# ==============================================================================
# CAMERA PARAMETER CALCULATIONS
# ==============================================================================

def focal_length_from_fov_and_resolution(fov_radians: float, 
                                                   resolution_pixels: int, 
                                                   pixel_size_meters: float) -> float:
    """
    Calculate focal length from field of view, resolution, and pixel size.
    Based on the template: focal_length = (resolution * pixel_size) / 2 / tan(half_fov)
    
    Args:
        fov_radians: Half field of view in radians
        resolution_pixels: Sensor resolution in pixels (square sensor)
        pixel_size_meters: Physical pixel size in meters
        
    Returns:
        Focal length in meters
    """
    return (resolution_pixels * pixel_size_meters) / 2 / np.tan(fov_radians / 2)
