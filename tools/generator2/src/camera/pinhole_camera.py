#!/usr/bin/env python3
"""
Pinhole Camera Model Implementation

This module provides a finite projective pinhole camera model for 3D to 2D
coordinate transformations. The implementation follows standard computer vision
conventions and includes proper handling of camera pose, intrinsic parameters,
and perspective projection.

Classes:
    Camera: Finite projective pinhole camera with configurable parameters

The camera model uses the following coordinate systems:
- World coordinates: 3D points in the global reference frame
- Camera coordinates: 3D points relative to camera center with Z-axis forward
- Image coordinates: 2D pixel coordinates in the output image

Transformation pipeline:
    World → Camera → Image
    1. Translation and rotation (world to camera)
    2. Perspective projection (camera to image)
    3. Pixel coordinate mapping (metric to discrete)
"""

import numpy as np
from typing import Tuple

# Default camera parameters
DEFAULT_FOCAL_LENGTH = 35.0  # mm
DEFAULT_PIXEL_SIZE = 0.01    # mm
DEFAULT_IMAGE_WIDTH = 1920   # pixels
DEFAULT_IMAGE_HEIGHT = 1080  # pixels


class Camera:
    """
    Finite projective pinhole camera model.
    
    This class implements a standard pinhole camera with configurable intrinsic
    and extrinsic parameters. It supports 3D to 2D projection with proper handling
    of camera pose (position and orientation) and sensor characteristics.
    
    The camera uses a right-handed coordinate system where:
    - X-axis points right
    - Y-axis points up  
    - Z-axis points forward (into the scene)
    
    Attributes:
        position (np.ndarray): Camera position [x, y, z] in world coordinates (meters)
        orientation (np.ndarray): Camera orientation [yaw, pitch, roll] in degrees
        focal_length (float): Focal length in meters
        pixel_size (float): Physical pixel size in meters
        image_width (int): Image width in pixels
        image_height (int): Image height in pixels
        rotation_matrix (np.ndarray): 3x3 rotation matrix for world-to-camera transform
        
    Examples:
        >>> # Create camera 10km from Earth, looking toward center
        >>> camera = Camera(
        ...     position=[-10000000, 0, 0],
        ...     orientation=[0, -90, 0],
        ...     focal_length=35.0,
        ...     pixel_size=0.01,
        ...     image_width=1920,
        ...     image_height=1080
        ... )
        >>> 
        >>> # Transform world points to camera coordinates
        >>> world_points = np.array([[0, 0, 0], [6371000, 0, 0]])  # Earth center and surface
        >>> camera_points = camera.world_to_camera(world_points)
        >>> 
        >>> # Project to image coordinates
        >>> image_coords, valid_mask = camera.project_to_image(camera_points)
    """
    
    def __init__(self, position: np.ndarray, orientation: np.ndarray, 
                 focal_length: float = DEFAULT_FOCAL_LENGTH, 
                 pixel_size: float = DEFAULT_PIXEL_SIZE,
                 image_width: int = DEFAULT_IMAGE_WIDTH, 
                 image_height: int = DEFAULT_IMAGE_HEIGHT):
        """
        Initialize the pinhole camera with specified parameters.
        
        Args:
            position: Camera position [x, y, z] in meters (world coordinates)
            orientation: Camera orientation [yaw, pitch, roll] in degrees (Euler angles)
            focal_length: Focal length in millimeters
            pixel_size: Physical pixel size in millimeters  
            image_width: Image sensor width in pixels
            image_height: Image sensor height in pixels
            
        Note:
            Focal length and pixel size are converted from millimeters to meters
            internally for consistent SI units throughout the calculations.
        """
        # Store camera parameters
        self.position = np.array(position, dtype=float)
        self.orientation = np.array(orientation, dtype=float)
        
        # Convert to SI units (meters)
        self.focal_length = focal_length / 1000.0  # mm to m
        self.pixel_size = pixel_size / 1000.0      # mm to m
        
        # Image sensor parameters
        self.image_width = int(image_width)
        self.image_height = int(image_height)
        
        # Validate parameters
        self._validate_parameters()
        
        # Compute camera transformation matrix
        self.rotation_matrix = self._create_rotation_matrix(orientation)
        
    def _validate_parameters(self):
        """Validate camera parameters for physical consistency."""
        if self.focal_length <= 0:
            raise ValueError(f"Focal length must be positive, got {self.focal_length*1000:.3f}mm")
            
        if self.pixel_size <= 0:
            raise ValueError(f"Pixel size must be positive, got {self.pixel_size*1000:.6f}mm")
            
        if self.image_width <= 0 or self.image_height <= 0:
            raise ValueError(f"Image dimensions must be positive, got {self.image_width}×{self.image_height}")
    
    def _create_rotation_matrix(self, orientation: np.ndarray) -> np.ndarray:
        """
        Create 3D rotation matrix from Euler angles (yaw, pitch, roll).
        
        The rotation sequence follows the ZYX convention (yaw-pitch-roll):
        1. Yaw (rotation around Z-axis)
        2. Pitch (rotation around Y-axis) 
        3. Roll (rotation around X-axis)
        
        Args:
            orientation: [yaw, pitch, roll] angles in degrees
            
        Returns:
            3x3 rotation matrix for transforming world to camera coordinates
            
        Note:
            The resulting matrix transforms vectors from world coordinates to
            camera coordinates: v_cam = R @ v_world
        """
        yaw, pitch, roll = np.radians(orientation)
        
        # Individual rotation matrices
        # Yaw rotation (around Z-axis)
        R_yaw = np.array([
            [np.cos(yaw), -np.sin(yaw), 0],
            [np.sin(yaw), np.cos(yaw), 0],
            [0, 0, 1]
        ])
        
        # Pitch rotation (around Y-axis)
        R_pitch = np.array([
            [np.cos(pitch), 0, np.sin(pitch)],
            [0, 1, 0],
            [-np.sin(pitch), 0, np.cos(pitch)]
        ])
        
        # Roll rotation (around X-axis)
        R_roll = np.array([
            [1, 0, 0],
            [0, np.cos(roll), -np.sin(roll)],
            [0, np.sin(roll), np.cos(roll)]
        ])
        
        # Combined rotation: R = R_yaw * R_pitch * R_roll
        return R_yaw @ R_pitch @ R_roll
    
    def world_to_camera(self, world_points: np.ndarray) -> np.ndarray:
        """
        Transform points from world coordinates to camera coordinates.
        
        This transformation consists of:
        1. Translation: Move origin to camera position
        2. Rotation: Align axes with camera orientation
        
        Args:
            world_points: Array of 3D world coordinates [N, 3]
            
        Returns:
            Array of 3D camera coordinates [N, 3]
            
        Note:
            In camera coordinates, the Z-axis points forward (positive Z means
            in front of the camera), which is essential for proper projection.
        """
        # Validate input
        world_points = np.asarray(world_points)
        if world_points.ndim != 2 or world_points.shape[1] != 3:
            raise ValueError(f"Expected points array of shape [N, 3], got {world_points.shape}")
        
        # Translate to camera origin
        translated = world_points - self.position
        
        # Rotate to camera coordinate system
        # Use transpose of rotation matrix for inverse transformation
        camera_points = translated @ self.rotation_matrix.T
        
        return camera_points
    
    def project_to_image(self, camera_points: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Project 3D camera coordinates to 2D image pixel coordinates.
        
        This performs perspective projection followed by conversion to discrete
        pixel coordinates. Points behind the camera (negative Z) are marked as invalid.
        
        Args:
            camera_points: Array of 3D camera coordinates [N, 3]
            
        Returns:
            Tuple containing:
                - projected_points: 2D image coordinates [N, 2] in pixels
                - valid_mask: Boolean array [N] indicating points in front of camera
                
        Note:
            Image coordinate system has origin at top-left corner:
            - X increases to the right
            - Y increases downward
            - (0, 0) is the top-left pixel
            - (width-1, height-1) is the bottom-right pixel
        """
        # Validate input
        camera_points = np.asarray(camera_points)
        if camera_points.ndim != 2 or camera_points.shape[1] != 3:
            raise ValueError(f"Expected points array of shape [N, 3], got {camera_points.shape}")
        
        # Check if points are in front of camera (positive Z in camera coordinates)
        valid_mask = camera_points[:, 2] > 0
        
        # Initialize output array
        num_points = len(camera_points)
        projected = np.zeros((num_points, 2))
        
        if np.any(valid_mask):
            valid_points = camera_points[valid_mask]
            
            # Perspective projection: (X, Y, Z) → (X/Z, Y/Z) * focal_length
            x_proj = (self.focal_length * valid_points[:, 0]) / valid_points[:, 2]
            y_proj = (self.focal_length * valid_points[:, 1]) / valid_points[:, 2]
            
            # Convert from metric coordinates to pixel coordinates
            # Apply sensor scaling and center offset
            x_pixel = (x_proj / self.pixel_size) + (self.image_width / 2)
            y_pixel = (y_proj / self.pixel_size) + (self.image_height / 2)
            
            # Store projected coordinates
            projected[valid_mask] = np.column_stack([x_pixel, y_pixel])
        
        return projected, valid_mask
    
    def get_field_of_view(self) -> Tuple[float, float]:
        """
        Calculate the camera's field of view angles.
        
        Returns:
            Tuple of (horizontal_fov, vertical_fov) in degrees
        """
        # Calculate sensor dimensions in meters
        sensor_width = self.image_width * self.pixel_size
        sensor_height = self.image_height * self.pixel_size
        
        # Calculate field of view angles
        horizontal_fov = 2 * np.arctan(sensor_width / (2 * self.focal_length))
        vertical_fov = 2 * np.arctan(sensor_height / (2 * self.focal_length))
        
        return np.degrees(horizontal_fov), np.degrees(vertical_fov)
    
    def get_diagonal_fov(self) -> float:
        """
        Calculate the diagonal field of view angle.
        
        Returns:
            Diagonal field of view in degrees
        """
        # Calculate sensor diagonal
        sensor_width = self.image_width * self.pixel_size
        sensor_height = self.image_height * self.pixel_size
        sensor_diagonal = np.sqrt(sensor_width**2 + sensor_height**2)
        
        # Calculate diagonal FOV
        diagonal_fov = 2 * np.arctan(sensor_diagonal / (2 * self.focal_length))
        
        return np.degrees(diagonal_fov)
    
    def __str__(self) -> str:
        """String representation of camera parameters."""
        h_fov, v_fov = self.get_field_of_view()
        d_fov = self.get_diagonal_fov()
        
        return (f"Camera(\n"
                f"  position={self.position},\n"
                f"  orientation={self.orientation}°,\n"
                f"  focal_length={self.focal_length*1000:.1f}mm,\n"
                f"  pixel_size={self.pixel_size*1000:.3f}mm,\n"
                f"  resolution={self.image_width}×{self.image_height},\n"
                f"  fov={h_fov:.1f}°×{v_fov:.1f}° (diagonal: {d_fov:.1f}°)\n"
                f")")
    
    def __repr__(self) -> str:
        """Detailed representation for debugging."""
        return self.__str__()
