"""
Parameter generation functions for satellite imaging simulations.

This module provides functions to generate camera parameters, positions, and
orientations for systematic imaging simulations based on orbital geometry.
"""

import numpy as np
from typing import Tuple, List
from .constants import EARTH_MEAN_RADIUS

def generate_camera_angles(distance: float, 
                          focal_length: float,
                          resolution: int,
                          pixel_size: float,
                          num_cam_angles: int = 4,
                          num_spins: int = 1,
                          padding: float = 0.1,
                          earth_radius: float = EARTH_MEAN_RADIUS) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Generate camera angles for systematic Earth imaging based on geometric constraints.
    
    This function calculates camera orientations that ensure Earth coverage while
    respecting field of view limitations and orbital geometry.
    
    Args:
        distance: Camera distance from Earth center in meters
        focal_length: Camera focal length in meters
        resolution: Sensor resolution in pixels (square sensor)
        pixel_size: Physical pixel size in meters
        num_cam_angles: Number of camera angles to generate
        num_spins: Number of spin orientations (currently supports 1)
        padding: Padding factor for angle calculation (0.0 to 1.0)
        earth_radius: Earth radius in meters (default: EARTH_MEAN_RADIUS)
        
    Returns:
        Tuple of (cam_angles_degrees, y_angles, x_angles)
        - cam_angles_degrees: Camera angles in degrees
        - y_angles: Y orientation components 
        - x_angles: X orientation components
    """
    # Calculate half field of view from camera parameters
    half_fov = np.arctan((resolution * pixel_size) / (2 * focal_length))
    
    # Calculate Earth's inscribed angle as seen from the camera
    inscribed_angle = np.arcsin(earth_radius / distance)
    
    # Calculate angle range with padding
    lower = inscribed_angle - half_fov * (1 - padding)
    upper = inscribed_angle + half_fov * (1 - padding)
    
    # Ensure lower bound doesn't go below the absolute minimum
    absolute_lower = -inscribed_angle + half_fov * (1 + padding)
    lower = max(lower, absolute_lower)
    
    # Generate camera angles
    cam_angles = np.linspace(lower, upper, num=num_cam_angles)
    cam_angles_degrees = np.degrees(cam_angles)
    
    # Calculate spin orientation (currently supports single spin at π)
    spin = np.pi
    y_angles = np.sin(spin) * cam_angles_degrees
    x_angles = np.cos(spin) * cam_angles_degrees
    
    return cam_angles_degrees, y_angles, x_angles

def generate_camera_positions_and_orientations(distance: float,
                                             focal_length: float, 
                                             resolution: int,
                                             pixel_size: float,
                                             num_cam_angles: int = 4,
                                             padding: float = 0.1,
                                             earth_radius: float = EARTH_MEAN_RADIUS) -> List[Tuple[str, str]]:
    """
    Generate camera positions and orientations as formatted strings.
    
    Args:
        distance: Camera distance from Earth center in meters
        focal_length: Camera focal length in meters  
        resolution: Sensor resolution in pixels (square sensor)
        pixel_size: Physical pixel size in meters
        num_cam_angles: Number of camera angles to generate
        padding: Padding factor for angle calculation
        earth_radius: Earth radius in meters
        
    Returns:
        List of (position_str, orientation_str) tuples ready for command line use
    """
    cam_angles_deg, y_angles, x_angles = generate_camera_angles(
        distance, focal_length, resolution, pixel_size, 
        num_cam_angles, padding=padding, earth_radius=earth_radius
    )
    
    positions_orientations = []
    
    for i in range(num_cam_angles):
        # Position: camera at specified distance on negative X axis
        position = f"{-distance} 0 0"
        
        # Orientation: calculated angles with zero roll
        orientation = f"{y_angles[i]:.2f} {x_angles[i]:.2f} 0"
        
        positions_orientations.append((position, orientation))
    
    return positions_orientations

def calculate_earth_coverage_params(distance: float, 
                                   earth_radius: float = EARTH_MEAN_RADIUS) -> dict:
    """
    Calculate useful parameters for Earth coverage at a given distance.
    
    Args:
        distance: Camera distance from Earth center in meters
        earth_radius: Earth radius in meters
        
    Returns:
        Dictionary with coverage parameters
    """
    inscribed_angle = np.arcsin(earth_radius / distance)
    angular_diameter = 2 * inscribed_angle
    
    return {
        'inscribed_angle_rad': inscribed_angle,
        'inscribed_angle_deg': np.degrees(inscribed_angle),
        'angular_diameter_rad': angular_diameter,
        'angular_diameter_deg': np.degrees(angular_diameter),
        'distance_km': distance / 1000,
        'altitude_km': (distance - earth_radius) / 1000
    }

def print_simulation_summary(distance: float,
                           focal_length: float,
                           resolution: int, 
                           pixel_size: float,
                           num_cam_angles: int = 4,
                           earth_radius: float = EARTH_MEAN_RADIUS):
    """
    Print a summary of simulation parameters.
    
    Args:
        distance: Camera distance from Earth center in meters
        focal_length: Camera focal length in meters
        resolution: Sensor resolution in pixels
        pixel_size: Physical pixel size in meters  
        num_cam_angles: Number of camera angles
        earth_radius: Earth radius in meters
    """
    # Calculate FOV
    half_fov = np.arctan((resolution * pixel_size) / (2 * focal_length))
    full_fov_deg = np.degrees(2 * half_fov)
    
    # Get Earth coverage params
    coverage = calculate_earth_coverage_params(distance, earth_radius)
    
    # Generate angles
    cam_angles_deg, y_angles, x_angles = generate_camera_angles(
        distance, focal_length, resolution, pixel_size, num_cam_angles
    )
    
    print(f"🛰️ Simulation Parameters Summary")
    print(f"=" * 40)
    print(f"📏 Camera Configuration:")
    print(f"   Distance: {distance/1000000:.1f} Mm ({coverage['altitude_km']:.0f} km altitude)")
    print(f"   Focal length: {focal_length*1000:.1f} mm")
    print(f"   Resolution: {resolution}×{resolution} pixels")
    print(f"   Pixel size: {pixel_size*1000000:.1f} μm")
    print(f"   Field of view: {full_fov_deg:.1f}°")
    
    print(f"\n🌍 Earth Coverage:")
    print(f"   Earth angular size: {coverage['angular_diameter_deg']:.2f}°")
    print(f"   Inscribed angle: {coverage['inscribed_angle_deg']:.2f}°")
    
    print(f"\n📐 Generated Camera Angles ({num_cam_angles} positions):")
    for i, (angle, y_ang, x_ang) in enumerate(zip(cam_angles_deg, y_angles, x_angles)):
        print(f"   #{i+1}: angle={angle:.2f}°, orientation=({y_ang:.2f}°, {x_ang:.2f}°, 0°)")

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
    
    print_simulation_summary(distance, focal_length, resolution, pixel_size)
    
    print(f"\n🎮 Generated Position/Orientation Pairs:")
    positions_orientations = generate_camera_positions_and_orientations(
        distance, focal_length, resolution, pixel_size
    )
    
    for i, (pos, orient) in enumerate(positions_orientations):
        print(f"   #{i+1}: position='{pos}', orientation='{orient}'")

if __name__ == "__main__":
    example_usage()
