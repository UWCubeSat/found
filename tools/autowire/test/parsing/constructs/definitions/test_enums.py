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
, set_parent)


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
        return set_parent(Enum(name, ENUM_VALUES), self.file)
    

    
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
