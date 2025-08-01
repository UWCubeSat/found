"""
Generator2 Tool - Earth Image Generation

A command-line utility for generating synthetic 2D images of Earth modeled as a WGS84 
oblate spheroid, as seen from a finite projective pinhole camera at a specified 
position and orientation.

This package provides:
- WGS84 Earth spheroid modeling with geometric calculations
- Finite projective pinhole camera model with full pose control  
- High-quality Earth image rendering with solid geometry
- Accurate pixel-level analysis for statistics
- Comprehensive error handling and validation

Modules:
    models: Earth geometric models (WGS84Spheroid)
    camera: Camera models and projections (Camera)
    rendering: Image generation and visualization (EarthRenderer)
    analysis: Pixel-level image analysis (ImageAnalyzer)

Examples:
    Basic usage:
    $ python -m tools.generator2 --position -10000000 0 0 --orientation 0 -90 0
    
    Custom parameters:
    $ python -m tools.generator2 --position -8000000 0 0 --orientation 0 -90 0 \\
        --focal-length 25 --pixel-size 0.02 --image-size 1280 720 \\
        --output earth_view.png
"""

from .models import WGS84Spheroid
from .camera import Camera
from .rendering import EarthRenderer
from .analysis import ImageAnalyzer

__version__ = "1.0.0"
__author__ = "UWCubeSat Husky Satellite Lab"

__all__ = [
    'WGS84Spheroid',
    'Camera', 
    'EarthRenderer',
    'ImageAnalyzer'
]
