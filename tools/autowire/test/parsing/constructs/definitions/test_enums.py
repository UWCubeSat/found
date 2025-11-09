"""Tests for enum construct classes.

This module tests the Enum construct which represents C++ enum and enum class
declarations. Tests cover initialization with various parameters, underlying
types, and value collections.
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.enums import Enum
from src.parsing.constructs.types.types import Type
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, ENUM_NAME, INT_TYPE, ENUM_VALUES
)


class TestEnum(unittest.TestCase):
    """Test cases for Enum construct.
    
    Tests enum initialization with different configurations including
    enum classes, underlying types, and value collections.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_enum(self, name=ENUM_NAME):
        """Helper to create a simple enum with default values.
        
        Args:
            name (str): Name for the enum, defaults to ENUM_NAME
            
        Returns:
            Enum: Simple enum with standard test values
        """
        return Enum(self.file, name, ENUM_VALUES)
    
    def test_enum_initialization_simple(self):
        """Test simple enum initialization with default parameters."""
        enum = self.create_simple_enum()
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': ENUM_NAME,
            'values': ENUM_VALUES,
            'is_class': False,
            'underlying_type': None,
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, enum.__dict__)
    
    def test_enum_class_initialization(self):
        """Test enum class initialization with is_class=True."""
        enum = Enum(self.file, ENUM_NAME, ENUM_VALUES, is_class=True)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': ENUM_NAME,
            'values': ENUM_VALUES,
            'is_class': True,
            'underlying_type': None,
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, enum.__dict__)
    
    def test_enum_with_underlying_type(self):
        """Test enum with specified underlying type."""
        underlying_type = Type(self.file, INT_TYPE)
        enum = Enum(self.file, ENUM_NAME, ENUM_VALUES, underlying_type=underlying_type)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': ENUM_NAME,
            'values': ENUM_VALUES,
            'is_class': False,
            'underlying_type': underlying_type,
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, enum.__dict__)
    
    def test_enum_values_list(self):
        """Test enum values are stored and accessible correctly."""
        enum = self.create_simple_enum()
        
        expected = {
            'values_count': 3,
            'has_success': True,
            'has_error': True,
            'has_pending': True
        }
        actual = {
            'values_count': len(enum.values),
            'has_success': "SUCCESS" in enum.values,
            'has_error': "ERROR" in enum.values,
            'has_pending': "PENDING" in enum.values
        }
        
        self.assertEqual(expected, actual)


if __name__ == '__main__':
    unittest.main()