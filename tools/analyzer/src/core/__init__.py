"""
Core analyzer functionality for .found files.
"""

from .analyzer import FoundFileAnalyzer
from .constants import MAGIC_NUMBER, HEADER_SIZE, QUATERNION_SIZE, POSITION_RECORD_SIZE

__all__ = [
    'FoundFileAnalyzer',
    'MAGIC_NUMBER',
    'HEADER_SIZE', 
    'QUATERNION_SIZE',
    'POSITION_RECORD_SIZE'
]
