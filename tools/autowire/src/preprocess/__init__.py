"""
Preprocessing module for macro-aware file processing.
"""

from .constructs.parameterized_file import ParameterizedFile
from .preprocessor.parameterized_file_cache import ParameterizedFileCache

__all__ = ['parameterized_file', 'ParameterizedFileCache', 'ParameterizedFile']