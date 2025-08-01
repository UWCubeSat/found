"""
Camera models and projection systems for the generator2 tool.

This module contains classes for modeling finite projective pinhole cameras
and performing 3D to 2D coordinate transformations.
"""

from .pinhole_camera import Camera

__all__ = ['Camera']
