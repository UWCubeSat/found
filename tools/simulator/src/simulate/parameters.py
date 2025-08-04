"""
Parameter generation functions for satellite imaging simulations.

This module provides functions to generate camera parameters, positions, and
orientations for systematic imaging simulations based on orbital geometry.
"""

import numpy as np
from typing import Tuple, List
from common.constants import EARTH_MEAN_RADIUS
from common.utility import focal_length_from_fov_and_resolution

def generate_camera_angles(distance: float, 
                          fov: float,
                          num_cam_angles: int = 4,
                          num_spins: int = 4,
                          padding: float = 0.1,
                          earth_radius: float = EARTH_MEAN_RADIUS) -> Tuple[np.ndarray, np.ndarray]:
    """
    Generate camera angles for systematic Earth imaging based on geometric constraints.
    
    This function calculates camera orientations that ensure Earth coverage while
    respecting field of view limitations and orbital geometry.
    
    Args:
        distance: Camera distance from Earth center in meters
        fov: Camera field of view in radians
        num_cam_angles: Number of camera angles to generate
        num_spins: Number of spin orientations around the orbit
        padding: Padding factor for angle calculation (0.0 to 1.0)
        earth_radius: Earth radius in meters (default: EARTH_MEAN_RADIUS)
        
    Returns:
        Tuple of (y_angles, x_angles)
        - y_angles: Y orientation components 
        - x_angles: X orientation components
    """
    # Calculate half field of view from camera parameters
    half_fov = fov / 2

    # Calculate Earth's inscribed angle as seen from the camera
    inscribed_angle = np.arcsin(earth_radius / distance)
    
    # Calculate angle range with padding
    lower = inscribed_angle - half_fov * (1 - padding)
    upper = inscribed_angle + half_fov * (1 - padding)
    
    # Ensure lower bound doesn't go below the absolute minimum
    absolute_lower = -inscribed_angle + half_fov * (1 + padding)
    lower = max(lower, absolute_lower)
    
    # Generate camera angles in degrees
    cam_angles_degrees = np.degrees(np.linspace(lower, upper, num=num_cam_angles))

    # Calculate spin orientations for multiple spins
    spins = np.linspace(0, 2 * np.pi, num_spins, endpoint=False)  # Multiple spin orientations
    
    # Create all combinations of camera angles and spins
    all_y_angles = []
    all_x_angles = []
    
    for spin in spins:
        for cam_angle in cam_angles_degrees:
            all_y_angles.append(np.sin(spin) * cam_angle)
            all_x_angles.append(np.cos(spin) * cam_angle)
    
    return np.array(all_y_angles), np.array(all_x_angles)

def generate_command_flags(distance: float,
                          fov: float,
                          num_cam_angles: int = 4,
                          num_spins: int = 4,
                          padding: float = 0.1,
                          earth_radius: float = EARTH_MEAN_RADIUS) -> List[List[str]]:
    """
    Generate command-line flags for running the generator tool.
    
    Args:
        distance: Camera distance from Earth center in meters
        fov: Camera field of view in radians
        num_cam_angles: Number of camera angles to generate
        num_spins: Number of spin orientations around the orbit
        padding: Padding factor for angle calculation
        earth_radius: Earth radius in meters
        
    Returns:
        List of command flag lists, each ready for subprocess.run()
    """
    # First get the position/orientation pairs
    y_angles, x_angles = generate_camera_angles(
        distance, fov, num_cam_angles, num_spins, padding, earth_radius
    )
    
    flag_list = []
    
    for i in range(len(y_angles)):
        # Position: camera at specified distance on negative X axis
        position = f"{-distance} 0 0"
        
        # Orientation: calculated angles with zero roll
        orientation = f"{y_angles[i]:.2f} {x_angles[i]:.2f} 0"
        
        # Build command flags list
        cmd = [
            "--position", *position.split(),
            "--orientation", *orientation.split(),
        ]
        flag_list.append(cmd)

    print(flag_list)  # Show first 2 commands for brevity
    return flag_list

# Example usage function
def example_usage():
    """Demonstrate the parameter generation functions."""
    print("🎯 Parameter Generation Example")
    print("=" * 50)
    
    # Example parameters matching test.py
    distance = 10000000  # 10 Mm
    resolution = 6000    # 6000x6000 pixels
    pixel_size = 20e-6   # 20 μm
    
    # Calculate focal length using the template formula
    half_fov = np.pi / 4  # 45 degrees
    focal_length = (resolution * pixel_size) / 2 / np.tan(half_fov)
    
    print(f"📐 Camera Parameters:")
    print(f"   Distance: {distance/1000000:.1f} Mm")
    print(f"   Focal length: {focal_length*1000:.1f} mm")
    print(f"   Resolution: {resolution}x{resolution} px")
    print(f"   FOV: {np.degrees(2 * half_fov):.1f}°")
    
    # Generate command flags (smaller example)
    print(f"\n🚀 Generated Commands (2 angles × 2 spins = 4 total):")
    commands = generate_command_flags(
        distance, focal_length, resolution, pixel_size,
        num_cam_angles=2, num_spins=2,
        output_prefix="test_output"
    )
    
    for i, cmd in enumerate(commands):
        print(f"#{i+1}: {' '.join(cmd)}")

if __name__ == "__main__":
    example_usage()
