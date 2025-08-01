"""
Camera and Orbital Constants for Satellite Imaging Simulations

This module contains commonly used camera parameters and orbital ranges for
Low Earth Orbit (LEO) satellite imaging simulations.
"""

import numpy as np
from typing import Dict, List, Tuple, NamedTuple

# ==============================================================================
# EARTH AND ORBITAL PARAMETERS
# ==============================================================================

# Earth physical parameters (WGS84)
EARTH_EQUATORIAL_RADIUS = 6378137.0  # meters
EARTH_POLAR_RADIUS = 6356752.3       # meters  
EARTH_MEAN_RADIUS = 6371000.0        # meters (simplified spherical model)

# Low Earth Orbit (LEO) distance ranges
LEO_ALTITUDE_MIN = 160000      # 160 km - minimum sustainable altitude
LEO_ALTITUDE_MAX = 2000000     # 2000 km - upper LEO boundary
LEO_ALTITUDE_TYPICAL = 400000  # 400 km - ISS altitude

# Common LEO altitudes (above Earth surface)
LEO_ALTITUDES = {
    'very_low': 200000,     # 200 km - minimum operational
    'low': 400000,          # 400 km - ISS, most satellites
    'medium': 800000,       # 800 km - sun-synchronous orbits
    'high': 1200000,        # 1200 km - polar orbits
    'upper': 1600000,       # 1600 km - upper LEO
}

# Convert altitudes to distances from Earth center
LEO_DISTANCES = {
    name: altitude + EARTH_MEAN_RADIUS 
    for name, altitude in LEO_ALTITUDES.items()
}

# ==============================================================================
# SIMULATION PARAMETER RANGES
# ==============================================================================

# Common simulation parameter ranges
SIMULATION_RANGES = {
    'distances': {
        'close_leo': np.linspace(LEO_DISTANCES['very_low'], LEO_DISTANCES['low'], 5),
        'mid_leo': np.linspace(LEO_DISTANCES['low'], LEO_DISTANCES['medium'], 5),
        'full_leo': np.linspace(LEO_DISTANCES['very_low'], LEO_DISTANCES['upper'], 10),
    },
    
    'fovs': {
        'narrow': np.linspace(5, 20, 5),        # degrees - telephoto/high resolution
        'standard': np.linspace(20, 60, 5),     # degrees - normal imaging
        'wide': np.linspace(60, 90, 5),         # degrees - wide area coverage
        'ultra_wide': np.linspace(90, 140, 5),  # degrees - horizon capture
        "all_wide": np.linspace(60, 140, 10),   # degrees - wide range
        'full_range': np.linspace(5, 140, 15),  # degrees - complete range
    },
    
    'resolutions': {
        'low': [128, 256, 512],                 # pixels (square) - fast simulation
        'medium': [1024, 2048, 4096],           # pixels (square) - balanced
        'high': [6000, 8192, 10000],            # pixels (square) - high detail
        'cubesat': [300, 600, 1200],            # pixels (square) - CubeSat typical
        'full_range': [128, 256, 512, 1024, 2048, 4096, 6000, 8192]  # complete range
    }
}

DEFAULT_SIMULATION = {
    'distances': 'mid_leo',  # Default distance range for common simulations
    'fovs': 'all_wide',       # Default FOV range for common simulations
    'resolutions': 'full_range'   # Default resolution range for common simulations
}

DEFAULT_PIXEL_SIZE = 20e-6  # Default pixel size (m)



