#!/usr/bin/env python3
"""
Earth Rendering Engine

This module provides the EarthRenderer class for generating synthetic images
of Earth as seen from space-based cameras. The renderer combines geometric
Earth models with camera projections to create realistic visualizations.

Features:
- Solid Earth disk rendering with proper geometric boundaries
- Horizon circle detection and visualization
- Statistical analysis of rendered content
- Clean image output without UI elements
- Support for various field of view configurations

Classes:
    EarthRenderer: Main rendering engine for Earth visualization

The rendering pipeline:
1. Generate Earth disk points within the camera's field of view
2. Compute exact horizon circle geometry  
3. Transform all points to camera coordinates
4. Project 3D points to 2D image plane
5. Filter points within image bounds
6. Render solid Earth geometry with matplotlib
7. Analyze final image for accurate statistics
"""

import logging
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, Tuple
from pathlib import Path

# Import our modules
from ..models import WGS84Spheroid
from ..camera import Camera
from ..analysis import ImageAnalyzer


class EarthRenderer:
    """
    Main rendering engine for generating synthetic Earth images.
    
    This class combines Earth geometric models with camera projections to
    create realistic images of Earth as seen from space. It handles the
    complete pipeline from 3D geometry generation to final image output
    with statistical analysis.
    
    Attributes:
        camera (Camera): The camera model for projection and positioning
        spheroid (WGS84Spheroid): Earth model for geometric calculations
        analyzer (ImageAnalyzer): Tool for analyzing rendered image statistics
        
    Examples:
        >>> from generator2.src.models import WGS84Spheroid
        >>> from generator2.src.camera import Camera
        >>> 
        >>> # Set up camera 10,000 km from Earth
        >>> camera = Camera(
        ...     position=[-10000000, 0, 0],
        ...     orientation=[0, -90, 0],
        ...     focal_length=35.0,
        ...     pixel_size=0.01,
        ...     image_width=1920,
        ...     image_height=1080
        ... )
        >>> spheroid = WGS84Spheroid()
        >>> renderer = EarthRenderer(camera, spheroid)
        >>> 
        >>> # Generate Earth image
        >>> stats = renderer.render_image("earth_view.png")
        >>> print(f"Earth fills {stats['earth_fill_percentage']:.2f}% of image")
    """
    
    def __init__(self, camera: Camera, spheroid: WGS84Spheroid):
        """
        Initialize the Earth renderer with camera and Earth models.
        
        Args:
            camera: Camera model for projection and positioning
            spheroid: Earth model for geometric calculations
        """
        self.camera = camera
        self.spheroid = spheroid
        self.analyzer = ImageAnalyzer()
        
        # Validate that camera is positioned outside Earth
        if not self.spheroid.is_position_valid(self.camera.position):
            distance = np.linalg.norm(self.camera.position)
            raise ValueError(f"Camera position is inside Earth! "
                           f"Distance from center: {distance:.0f}m, "
                           f"Earth radius: {self.spheroid.mean_radius:.0f}m")
    
    def render_image(self, output_path: str, num_earth_points: int = 8000, 
                    num_horizon_points: int = 720) -> Dict:
        """
        Render and save a synthetic Earth image.
        
        This method executes the complete rendering pipeline:
        1. Generate Earth surface points visible from camera
        2. Compute horizon circle geometry
        3. Transform to camera coordinates and project to image
        4. Render solid Earth geometry 
        5. Analyze final image for statistics
        
        Args:
            output_path: File path for saving the output image
            num_earth_points: Number of sample points on Earth's surface
            num_horizon_points: Number of points around horizon circle
            
        Returns:
            Dictionary containing detailed rendering statistics:
                - earth_fill_percentage: Percentage of image filled by Earth
                - earth_area_pixels: Number of pixels occupied by Earth
                - horizon_captured: Whether horizon is visible in image
                - horizon_capture_percentage: Percentage of horizon visible
                - angular_radius_degrees: Earth's angular size from camera
                - image_width, image_height: Output image dimensions
                
        Raises:
            ValueError: If Earth is not visible from camera position/orientation
            IOError: If output file cannot be written
        """
        logging.info("Generating Earth disk points...")
        try:
            earth_points, angular_radius = self.spheroid.get_earth_disk_points(
                self.camera.position, num_points=num_earth_points)
        except ValueError as e:
            logging.error(str(e))
            raise
        
        logging.info("Generating horizon circle...")
        horizon_points = self.spheroid.get_earth_horizon_circle(
            self.camera.position, num_points=num_horizon_points)
        
        logging.info("Transforming points to camera coordinates...")
        camera_earth = self.camera.world_to_camera(earth_points)
        camera_horizon = self.camera.world_to_camera(horizon_points)
        
        logging.info("Projecting points to image plane...")
        earth_proj, earth_valid = self.camera.project_to_image(camera_earth)
        horizon_proj, horizon_valid = self.camera.project_to_image(camera_horizon)
        
        # Filter points within image bounds
        earth_in_image, _ = self._filter_points_in_bounds(earth_proj, earth_valid)
        horizon_in_image, _ = self._filter_points_in_bounds(horizon_proj, horizon_valid)
        
        # Validate Earth visibility
        if len(earth_in_image) == 0:
            logging.error("No part of Earth is visible in the camera image!")
            raise ValueError("Earth is not visible in the camera field of view")
        
        # Calculate Earth geometry in image space
        earth_center, earth_radius = self._calculate_earth_geometry(
            earth_in_image, horizon_in_image)
        
        # Create and save the image
        logging.info("Creating image...")
        self._create_earth_image(output_path, earth_center, earth_radius, 
                                earth_in_image, horizon_in_image)
        
        # Analyze final image for accurate statistics
        logging.info("Analyzing rendered image...")
        analysis_results = self.analyzer.analyze_earth_fill(output_path)
        
        # Compile comprehensive statistics
        stats = self._compile_statistics(
            earth_in_image, horizon_in_image, horizon_points,
            angular_radius, earth_center, earth_radius, analysis_results)
        
        logging.info(f"Image saved successfully to {output_path}")
        return stats
    
    def _filter_points_in_bounds(self, projected_points: np.ndarray, 
                                valid_mask: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Filter projected points to those within image boundaries.
        
        Args:
            projected_points: 2D projected coordinates [N, 2]
            valid_mask: Boolean mask for points in front of camera [N]
            
        Returns:
            Tuple of (points_in_image, bounds_mask)
        """
        valid_points = projected_points[valid_mask]
        if len(valid_points) == 0:
            return np.array([]).reshape(0, 2), np.array([], dtype=bool)
        
        # Check image boundary conditions
        in_bounds = ((valid_points[:, 0] >= 0) & 
                    (valid_points[:, 0] < self.camera.image_width) &
                    (valid_points[:, 1] >= 0) & 
                    (valid_points[:, 1] < self.camera.image_height))
        
        return valid_points[in_bounds], in_bounds
    
    def _calculate_earth_geometry(self, earth_points: np.ndarray, 
                                 horizon_points: np.ndarray) -> Tuple[Tuple[float, float], float]:
        """
        Calculate Earth's center and radius in image coordinates.
        
        Args:
            earth_points: Earth surface points in image [N, 2]
            horizon_points: Horizon points in image [M, 2]
            
        Returns:
            Tuple of ((center_x, center_y), radius)
        """
        if len(horizon_points) > 10:
            # Use horizon to determine Earth's center and radius accurately
            center_x = np.mean(horizon_points[:, 0])
            center_y = np.mean(horizon_points[:, 1])
            
            # Calculate radius from horizon points
            distances = np.sqrt((horizon_points[:, 0] - center_x)**2 + 
                              (horizon_points[:, 1] - center_y)**2)
            radius = np.mean(distances)
            
        elif len(earth_points) > 0:
            # Estimate from visible Earth points when horizon not available
            center_x = np.mean(earth_points[:, 0])
            center_y = np.mean(earth_points[:, 1])
            
            # Estimate radius with safety margin for partial visibility
            distances = np.sqrt((earth_points[:, 0] - center_x)**2 + 
                              (earth_points[:, 1] - center_y)**2)
            radius = np.max(distances) * 1.2
            
        else:
            # Fallback values
            center_x = self.camera.image_width / 2
            center_y = self.camera.image_height / 2
            radius = 0
        
        return (center_x, center_y), radius
    
    def _create_earth_image(self, output_path: str, earth_center: Tuple[float, float],
                           earth_radius: float, earth_points: np.ndarray, 
                           horizon_points: np.ndarray):
        """
        Create and save the Earth image using matplotlib.
        
        Args:
            output_path: Path for saving the image
            earth_center: (center_x, center_y) of Earth in image coordinates
            earth_radius: Earth radius in pixels
            earth_points: Visible Earth surface points
            horizon_points: Visible horizon points
        """
        # Create figure with exact pixel dimensions
        fig = plt.figure(figsize=(self.camera.image_width/100, self.camera.image_height/100), dpi=100)
        ax = fig.add_axes([0, 0, 1, 1])  # Full figure, no margins
        
        # Configure image coordinate system
        ax.set_xlim(0, self.camera.image_width)
        ax.set_ylim(0, self.camera.image_height)
        ax.set_aspect('equal')
        ax.invert_yaxis()  # Image coordinates have Y pointing down
        
        # Remove all axes, ticks, labels for clean output
        ax.set_xticks([])
        ax.set_yticks([])
        ax.axis('off')
        
        # Set space background
        ax.set_facecolor('black')
        
        # Render Earth as solid disk if geometry is available
        if earth_radius > 0 and len(earth_points) > 0:
            center_x, center_y = earth_center
            
            # Create filled circle for Earth
            earth_circle = plt.Circle((center_x, center_y), earth_radius, 
                                    color='#4169E1', alpha=0.9)
            ax.add_patch(earth_circle)
            
            # Add horizon edge if visible
            if len(horizon_points) > 10:
                horizon_circle = plt.Circle((center_x, center_y), earth_radius, 
                                          fill=False, color='#87CEEB', linewidth=1, alpha=0.7)
                ax.add_patch(horizon_circle)
        else:
            # Fallback: render as discrete points if solid geometry fails
            if len(earth_points) > 0:
                ax.scatter(earth_points[:, 0], earth_points[:, 1], 
                          c='#4169E1', s=3, alpha=0.8)
        
        # Save with high quality and no padding
        logging.info(f"Saving image to {output_path}...")
        plt.savefig(output_path, dpi=150, facecolor='black', edgecolor='none',
                   bbox_inches='tight', pad_inches=0)
        plt.close()
    
    def _compile_statistics(self, earth_points: np.ndarray, horizon_points: np.ndarray,
                           total_horizon_points: np.ndarray, angular_radius: float,
                           earth_center: Tuple[float, float], earth_radius: float,
                           analysis_results: Dict) -> Dict:
        """
        Compile comprehensive rendering statistics.
        
        Args:
            earth_points: Visible Earth points
            horizon_points: Visible horizon points
            total_horizon_points: All generated horizon points
            angular_radius: Earth's angular radius from camera
            earth_center: Earth center in image coordinates
            earth_radius: Earth radius in pixels
            analysis_results: Results from image analysis
            
        Returns:
            Dictionary with complete statistics
        """
        # Basic counts
        horizon_captured = len(horizon_points) > 0
        horizon_capture_percentage = (len(horizon_points) / len(total_horizon_points)) * 100
        
        # Use analysis results for fill percentage if available
        if analysis_results and 'earth_fill_percentage' in analysis_results:
            earth_fill_percentage = analysis_results['earth_fill_percentage']
            earth_area_pixels = analysis_results.get('earth_area_pixels', len(earth_points))
            
            logging.info(f"Earth fills {earth_fill_percentage:.2f}% of the image "
                        f"(verified by pixel analysis: {earth_area_pixels:.0f} pixels)")
        else:
            # Fallback to geometric estimation
            total_pixels = self.camera.image_width * self.camera.image_height
            earth_area_pixels = len(earth_points)
            earth_fill_percentage = (earth_area_pixels / total_pixels) * 100
            
            logging.warning("Using geometric estimation for Earth fill percentage")
        
        # Log key statistics
        logging.info(f"Horizon capture: {horizon_capture_percentage:.1f}% "
                    f"({len(horizon_points)}/{len(total_horizon_points)} points)")
        
        if not horizon_captured:
            logging.warning("Earth's horizon is not captured in the image")
        
        # Compile final statistics dictionary
        return {
            'earth_fill_percentage': earth_fill_percentage,
            'earth_area_pixels': earth_area_pixels,
            'earth_radius_pixels': earth_radius,
            'earth_center': earth_center,
            'horizon_captured': horizon_captured,
            'horizon_capture_percentage': horizon_capture_percentage,
            'earth_points_in_image': len(earth_points),
            'horizon_points_in_image': len(horizon_points),
            'total_horizon_points': len(total_horizon_points),
            'angular_radius_degrees': np.degrees(angular_radius),
            'image_width': self.camera.image_width,
            'image_height': self.camera.image_height
        }
