#!/usr/bin/env python3
"""
Earth Model Implementation

This module provides the WGS84Spheroid class for modeling Earth as an oblate
spheroid according to the World Geodetic System 1984 (WGS84) standard.

The implementation includes methods for:
- Generating points on the visible Earth disk from a camera position
- Computing the exact horizon circle as seen from an observer
- Handling geometric calculations for Earth's shape and visibility

Constants:
    EARTH_EQUATORIAL_RADIUS: Earth's equatorial radius in meters (6,378,137 m)
    EARTH_POLAR_RADIUS: Earth's polar radius in meters (6,356,752.3 m)

Classes:
    WGS84Spheroid: Earth model with disk and horizon generation capabilities
"""

import numpy as np
from typing import Tuple

# WGS84 Earth parameters (in meters)
EARTH_EQUATORIAL_RADIUS = 6378137.0
EARTH_POLAR_RADIUS = 6356752.3


class WGS84Spheroid:
    """
    Represents Earth as a WGS84 oblate spheroid.
    
    This class models Earth according to the WGS84 standard, providing methods
    to generate geometric representations of Earth as seen from external camera
    positions. The implementation uses a spherical approximation for computational
    efficiency while maintaining the WGS84 parameters for reference.
    
    Attributes:
        equatorial_radius (float): Earth's equatorial radius in meters
        polar_radius (float): Earth's polar radius in meters  
        eccentricity_squared (float): Square of Earth's eccentricity
        mean_radius (float): Mean radius used for spherical approximation
    
    Examples:
        >>> spheroid = WGS84Spheroid()
        >>> camera_pos = np.array([-10000000, 0, 0])  # 10,000 km from Earth center
        >>> earth_points, angular_radius = spheroid.get_earth_disk_points(camera_pos)
        >>> horizon_points = spheroid.get_earth_horizon_circle(camera_pos)
    """
    
    def __init__(self):
        """Initialize the WGS84 spheroid with standard Earth parameters."""
        self.equatorial_radius = EARTH_EQUATORIAL_RADIUS
        self.polar_radius = EARTH_POLAR_RADIUS
        self.eccentricity_squared = 1 - (self.polar_radius / self.equatorial_radius) ** 2
        
        # Use spherical approximation for computational efficiency
        # This can be enhanced to full oblate spheroid math in future versions
        self.mean_radius = (2 * self.equatorial_radius + self.polar_radius) / 3
    
    def get_earth_disk_points(self, camera_position: np.ndarray, num_points: int = 1000) -> Tuple[np.ndarray, float]:
        """
        Generate points that represent the visible Earth disk from camera position.
        
        This method creates a set of 3D points that uniformly sample the Earth's
        surface within the angular disk as seen from the camera. The points are
        distributed using proper angular sampling to ensure uniform coverage of
        the visible hemisphere.
        
        Args:
            camera_position: Camera position [x, y, z] in meters relative to Earth center
            num_points: Number of sample points to generate on Earth's surface
            
        Returns:
            Tuple containing:
                - points (np.ndarray): Array of 3D points on Earth's surface [N, 3]
                - angular_radius (float): Angular radius of Earth as seen from camera (radians)
                
        Raises:
            ValueError: If camera position is inside Earth's radius
            
        Note:
            The method uses Monte Carlo sampling within the angular disk and then
            projects rays to find intersections with Earth's surface. This ensures
            proper geometric representation of the visible Earth hemisphere.
        """
        cam_distance = np.linalg.norm(camera_position)
        
        # Validate camera position
        if cam_distance <= self.mean_radius:
            raise ValueError(f"Camera is inside Earth! Distance from center: {cam_distance:.0f}m, "
                           f"Earth radius: {self.mean_radius:.0f}m")
        
        # Calculate the angular radius of Earth as seen from camera
        angular_radius = np.arcsin(self.mean_radius / cam_distance)
        
        # Vector from camera to Earth center (viewing direction)
        earth_center = np.array([0, 0, 0])
        camera_to_earth = earth_center - camera_position
        camera_to_earth = camera_to_earth / np.linalg.norm(camera_to_earth)
        
        # Create orthonormal basis vectors for the Earth disk plane
        # Choose perpendicular vector based on camera direction to avoid singularities
        if abs(camera_to_earth[2]) < 0.9:
            u = np.cross(camera_to_earth, [0, 0, 1])
        else:
            u = np.cross(camera_to_earth, [1, 0, 0])
        u = u / np.linalg.norm(u)
        v = np.cross(camera_to_earth, u)
        
        # Generate points uniformly within the Earth's angular disk
        points = []
        
        for i in range(num_points):
            # Random point within unit circle using proper uniform distribution
            # sqrt(r) ensures uniform area distribution in polar coordinates
            r = np.sqrt(np.random.uniform(0, 1))
            theta = np.random.uniform(0, 2 * np.pi)
            
            # Scale by angular radius to get actual angular coordinates
            angular_x = r * angular_radius * np.cos(theta)
            angular_y = r * angular_radius * np.sin(theta)
            
            # Convert angular coordinates to 3D direction vector
            # Using small angle approximation for computational efficiency
            direction = (camera_to_earth + 
                        angular_x * u + 
                        angular_y * v)
            direction = direction / np.linalg.norm(direction)
            
            # Find intersection of ray with Earth sphere
            # Solve: |camera_position + t * direction|^2 = mean_radius^2
            # This gives quadratic equation: at^2 + bt + c = 0
            a = np.dot(direction, direction)  # Should be 1 for unit vector
            b = 2 * np.dot(camera_position, direction)
            c = np.dot(camera_position, camera_position) - self.mean_radius**2
            
            discriminant = b**2 - 4*a*c
            if discriminant >= 0:
                # Take the closer intersection point (negative t value)
                t = (-b - np.sqrt(discriminant)) / (2*a)
                if t > 0:  # Ensure intersection is in front of camera
                    point = camera_position + t * direction
                    points.append(point)
        
        return np.array(points), angular_radius
    
    def get_earth_horizon_circle(self, camera_position: np.ndarray, num_points: int = 360) -> np.ndarray:
        """
        Generate points on the exact horizon circle of Earth as seen from camera.
        
        The horizon circle represents the boundary between the visible and hidden
        parts of Earth from the camera's perspective. This is computed using exact
        geometric relationships for a sphere, providing the true horizon edge.
        
        Args:
            camera_position: Camera position [x, y, z] in meters relative to Earth center
            num_points: Number of points to generate around the horizon circle
            
        Returns:
            Array of 3D points on the horizon circle [N, 3]
            
        Raises:
            ValueError: If camera position is inside Earth's radius
            
        Note:
            The horizon circle lies on a cone with its apex at the camera position.
            The cone's half-angle is determined by the geometric relationship between
            the camera distance and Earth's radius. This method computes the exact
            3D coordinates of points on this circle.
        """
        cam_distance = np.linalg.norm(camera_position)
        
        # Validate camera position
        if cam_distance <= self.mean_radius:
            raise ValueError(f"Camera is inside Earth! Distance from center: {cam_distance:.0f}m, "
                           f"Earth radius: {self.mean_radius:.0f}m")
        
        # Calculate horizon geometric parameters
        # Distance to horizon tangent points
        tangent_distance = np.sqrt(cam_distance**2 - self.mean_radius**2)
        
        # Angle between camera-Earth line and horizon tangent
        horizon_angle = np.arccos(self.mean_radius / cam_distance)
        
        # Vector from camera to Earth center
        earth_center = np.array([0, 0, 0])
        camera_to_earth = earth_center - camera_position
        camera_to_earth = camera_to_earth / np.linalg.norm(camera_to_earth)
        
        # Create orthonormal basis vectors for the horizon plane
        if abs(camera_to_earth[2]) < 0.9:
            u = np.cross(camera_to_earth, [0, 0, 1])
        else:
            u = np.cross(camera_to_earth, [1, 0, 0])
        u = u / np.linalg.norm(u)
        v = np.cross(camera_to_earth, u)
        
        # The horizon circle lies on a cone with half-angle = (π/2 - horizon_angle)
        # This cone has its apex at the camera and axis along camera_to_earth
        cone_half_angle = np.pi/2 - horizon_angle
        
        # Geometry of the horizon circle
        circle_radius = tangent_distance * np.sin(cone_half_angle)
        circle_center = camera_position + tangent_distance * np.cos(cone_half_angle) * camera_to_earth
        
        # Generate points uniformly around the horizon circle
        angles = np.linspace(0, 2 * np.pi, num_points, endpoint=False)
        horizon_points = []
        
        for angle in angles:
            # Point on circle using parametric representation
            point = (circle_center + 
                    circle_radius * np.cos(angle) * u + 
                    circle_radius * np.sin(angle) * v)
            horizon_points.append(point)
        
        return np.array(horizon_points)
    
    def get_angular_radius(self, camera_position: np.ndarray) -> float:
        """
        Calculate the angular radius of Earth as seen from camera position.
        
        Args:
            camera_position: Camera position [x, y, z] in meters
            
        Returns:
            Angular radius in radians
            
        Raises:
            ValueError: If camera position is inside Earth
        """
        cam_distance = np.linalg.norm(camera_position)
        
        if cam_distance <= self.mean_radius:
            raise ValueError("Camera is inside Earth!")
            
        return np.arcsin(self.mean_radius / cam_distance)
    
    def is_position_valid(self, position: np.ndarray) -> bool:
        """
        Check if a position is outside Earth (valid for camera placement).
        
        Args:
            position: Position [x, y, z] in meters
            
        Returns:
            True if position is outside Earth, False otherwise
        """
        distance = np.linalg.norm(position)
        return distance > self.mean_radius
