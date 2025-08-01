"""
Earth and geometric models for the generator2 tool.

This module contains classes for modeling Earth as a WGS84 oblate spheroid
and generating geometric representations for camera-based imaging.
"""

from .earth_model import WGS84Spheroid

__all__ = ['WGS84Spheroid']
