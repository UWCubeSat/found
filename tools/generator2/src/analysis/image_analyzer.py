#!/usr/bin/env python3
"""
Image Analysis Engine

This module provides tools for analyzing rendered Earth images to extract
accurate statistics about Earth coverage, pixel counts, and visual content.
The analysis uses computer vision techniques to ensure measurements reflect
the actual rendered content rather than geometric approximations.

Features:
- Pixel-level Earth vs space classification
- Accurate area calculations for continuous objects
- Multiple analysis backends (OpenCV, PIL) with fallback support
- Color detection with tolerance for compression artifacts
- Statistical validation and error reporting

Classes:
    ImageAnalyzer: Main engine for image content analysis

The analysis pipeline:
1. Load rendered image from file
2. Apply color detection to identify Earth pixels
3. Count Earth vs space pixels for accurate statistics
4. Validate results and provide confidence metrics
"""

import logging
import numpy as np
from typing import Dict, Optional, Union
from pathlib import Path


class ImageAnalyzer:
    """
    Image analysis engine for Earth coverage statistics.
    
    This class analyzes rendered Earth images to provide accurate measurements
    of Earth coverage, treating Earth as a continuous solid object rather than
    discrete sample points. Multiple analysis backends ensure robust operation
    across different environments.
    
    Attributes:
        earth_color_rgb (np.ndarray): RGB color values for Earth (#4169E1)
        analysis_methods (list): Available analysis backends in order of preference
        
    Examples:
        >>> analyzer = ImageAnalyzer()
        >>> results = analyzer.analyze_earth_fill("earth_image.png")
        >>> print(f"Earth fills {results['earth_fill_percentage']:.2f}% of image")
    """
    
    def __init__(self):
        """Initialize the image analyzer with Earth color parameters."""
        # Earth color: Royal Blue (#4169E1 = RGB(65, 105, 225))
        self.earth_color_rgb = np.array([65, 105, 225])
        
        # Analysis backends in order of preference
        self.analysis_methods = ['opencv', 'pil']
    
    def analyze_earth_fill(self, image_path: Union[str, Path]) -> Optional[Dict]:
        """
        Analyze an Earth image to determine accurate fill percentage.
        
        This method uses pixel-level analysis to count Earth vs space pixels,
        providing much more accurate statistics than geometric approximations.
        Multiple analysis backends are tried in order of preference.
        
        Args:
            image_path: Path to the rendered Earth image file
            
        Returns:
            Dictionary with analysis results:
                - earth_fill_percentage: Percentage of image filled by Earth
                - earth_area_pixels: Number of pixels identified as Earth
                - total_pixels: Total number of pixels in image
                - analysis_method: Backend used for analysis
                - confidence: Analysis confidence level (if available)
                
            Returns None if analysis fails for all backends.
            
        Raises:
            FileNotFoundError: If image file does not exist
        """
        image_path = Path(image_path)
        if not image_path.exists():
            raise FileNotFoundError(f"Image file not found: {image_path}")
        
        # Try analysis methods in order of preference
        for method in self.analysis_methods:
            try:
                if method == 'opencv':
                    result = self._analyze_with_opencv(image_path)
                elif method == 'pil':
                    result = self._analyze_with_pil(image_path)
                else:
                    continue
                
                if result is not None:
                    result['analysis_method'] = method
                    return result
                    
            except ImportError:
                logging.debug(f"Analysis method '{method}' not available (missing dependency)")
                continue
            except Exception as e:
                logging.warning(f"Analysis method '{method}' failed: {e}")
                continue
        
        logging.error("All image analysis methods failed")
        return None
    
    def _analyze_with_opencv(self, image_path: Path) -> Optional[Dict]:
        """
        Analyze image using OpenCV for color detection.
        
        OpenCV provides robust HSV color space analysis which is more reliable
        for color detection than RGB space, especially with compression artifacts.
        
        Args:
            image_path: Path to image file
            
        Returns:
            Analysis results dictionary or None if failed
        """
        try:
            import cv2
        except ImportError:
            raise ImportError("OpenCV (cv2) not available")
        
        # Load image
        image = cv2.imread(str(image_path))
        if image is None:
            logging.warning(f"OpenCV could not load image: {image_path}")
            return None
        
        # Convert to HSV for better color detection
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        # Define HSV range for Earth color
        # RGB(65, 105, 225) → HSV(225°, 71%, 88%)
        # OpenCV HSV ranges: H[0,179], S[0,255], V[0,255]
        # Convert: H = 225 * 179/360 ≈ 112, S = 71 * 255/100 ≈ 181, V = 88 * 255/100 ≈ 224
        lower_blue = np.array([100, 120, 150])  # Permissive lower bound
        upper_blue = np.array([130, 255, 255])  # Permissive upper bound
        
        # Create mask for Earth pixels
        earth_mask = cv2.inRange(hsv, lower_blue, upper_blue)
        
        # Count pixels
        earth_pixels = np.sum(earth_mask > 0)
        total_pixels = image.shape[0] * image.shape[1]
        earth_fill_percentage = (earth_pixels / total_pixels) * 100
        
        return {
            'earth_fill_percentage': earth_fill_percentage,
            'earth_area_pixels': earth_pixels,
            'total_pixels': total_pixels,
            'confidence': 'high'  # OpenCV HSV analysis is generally reliable
        }
    
    def _analyze_with_pil(self, image_path: Path) -> Optional[Dict]:
        """
        Analyze image using PIL with RGB color distance matching.
        
        PIL provides a fallback when OpenCV is not available. Uses Euclidean
        distance in RGB space to identify Earth pixels with appropriate tolerance.
        
        Args:
            image_path: Path to image file
            
        Returns:
            Analysis results dictionary or None if failed
        """
        try:
            from PIL import Image
        except ImportError:
            raise ImportError("PIL (Pillow) not available")
        
        # Load and convert image
        try:
            image = Image.open(image_path)
            if image.mode != 'RGB':
                image = image.convert('RGB')
        except Exception as e:
            logging.warning(f"PIL could not load image {image_path}: {e}")
            return None
        
        # Convert to numpy array
        img_array = np.array(image)
        
        # Calculate color distance from Earth blue
        color_diff = np.sqrt(np.sum((img_array - self.earth_color_rgb)**2, axis=2))
        
        # Count pixels close to Earth color (tolerance for compression artifacts)
        tolerance = 100  # Permissive tolerance for JPEG compression effects
        earth_pixels = np.sum(color_diff < tolerance)
        total_pixels = img_array.shape[0] * img_array.shape[1]
        earth_fill_percentage = (earth_pixels / total_pixels) * 100
        
        return {
            'earth_fill_percentage': earth_fill_percentage,
            'earth_area_pixels': earth_pixels,
            'total_pixels': total_pixels,
            'confidence': 'medium'  # RGB distance matching is less robust than HSV
        }
    
    def validate_analysis(self, results: Dict, expected_range: tuple = (0.0, 100.0)) -> bool:
        """
        Validate analysis results for reasonableness.
        
        Args:
            results: Analysis results dictionary
            expected_range: (min, max) acceptable fill percentage range
            
        Returns:
            True if results appear valid, False otherwise
        """
        if not results:
            return False
        
        fill_pct = results.get('earth_fill_percentage', -1)
        earth_pixels = results.get('earth_area_pixels', -1)
        total_pixels = results.get('total_pixels', -1)
        
        # Check basic validity
        if fill_pct < 0 or earth_pixels < 0 or total_pixels <= 0:
            return False
        
        # Check range
        if not (expected_range[0] <= fill_pct <= expected_range[1]):
            return False
        
        # Check consistency
        if total_pixels > 0:
            calculated_pct = (earth_pixels / total_pixels) * 100
            if abs(calculated_pct - fill_pct) > 0.01:  # Allow small floating point error
                return False
        
        return True
    
    def get_analysis_info(self) -> Dict:
        """
        Get information about available analysis methods.
        
        Returns:
            Dictionary with available methods and their status
        """
        info = {
            'available_methods': [],
            'preferred_method': None,
            'earth_color_rgb': self.earth_color_rgb.tolist()
        }
        
        # Check OpenCV availability
        try:
            import cv2
            info['available_methods'].append('opencv')
            if info['preferred_method'] is None:
                info['preferred_method'] = 'opencv'
        except ImportError:
            pass
        
        # Check PIL availability
        try:
            from PIL import Image
            info['available_methods'].append('pil')
            if info['preferred_method'] is None:
                info['preferred_method'] = 'pil'
        except ImportError:
            pass
        
        return info
