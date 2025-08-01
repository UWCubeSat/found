"""
Simulator tool source module.

This module provides utilities and constants for satellite imaging simulations,
including common camera parameters, orbital ranges, and imaging scenarios.
"""

from .constants import (
    # Earth and orbital parameters
    EARTH_EQUATORIAL_RADIUS,
    EARTH_POLAR_RADIUS, 
    EARTH_MEAN_RADIUS,
    LEO_ALTITUDES,
    LEO_DISTANCES,
    
    # Camera configurations
    CAMERA_SENSORS,
    CameraConfig,
    
    # Imaging scenarios
    IMAGING_SCENARIOS,
    SIMULATION_RANGES,
    
    # Utility functions
    get_camera_config,
    get_leo_distance,
    calculate_fov,
    calculate_ground_resolution,
    calculate_swath_width,
    suggest_focal_length,
    print_scenario_summary,
)

__all__ = [
    # Constants
    'EARTH_EQUATORIAL_RADIUS',
    'EARTH_POLAR_RADIUS',
    'EARTH_MEAN_RADIUS', 
    'LEO_ALTITUDES',
    'LEO_DISTANCES',
    'CAMERA_SENSORS',
    'IMAGING_SCENARIOS',
    'SIMULATION_RANGES',
    
    # Classes
    'CameraConfig',
    
    # Functions
    'get_camera_config',
    'get_leo_distance',
    'calculate_fov',
    'calculate_ground_resolution', 
    'calculate_swath_width',
    'suggest_focal_length',
    'print_scenario_summary',
]
