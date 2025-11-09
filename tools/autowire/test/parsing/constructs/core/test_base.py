"""Tests for base construct class.

This module tests the Construct base class functionality including:
- Parent-child relationships in construct hierarchy
- File path traversal through construct tree
- Root file discovery from any construct node
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.functions import Function
from src.parsing.constructs.types.types import Type
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, FUNCTION_NAME, INT_TYPE
)


class TestConstruct(unittest.TestCase):
    """Test cases for Construct base class functionality.
    
    Tests the fundamental construct hierarchy operations including
    parent relationships, file path resolution, and root file discovery.
    """
    
    def setUp(self):
        """Set up test fixtures with file and construct hierarchy."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_test_construct(self, parent):
        """Create a test construct (Function) with specified parent.
        
        Args:
            parent: Parent construct for the new function
            
        Returns:
            Function: Test function construct
        """
        return_type = Type(INT_TYPE)
        func = Function(FUNCTION_NAME, return_type)
        func.parent = parent
        return func
    
    def test_construct_initialization(self):
        """Test that construct properly stores its parent reference."""
        construct = self.create_test_construct(self.file)
        
        self.assertEqual(construct.parent, self.file)
    
    def test_get_file_path(self):
        """Test file path retrieval by traversing up construct hierarchy."""
        construct = self.create_test_construct(self.file)
        
        result = construct.get_file_path()
        
        self.assertEqual(result, SAMPLE_FILE_PATH)
    
    def test_get_root_file(self):
        """Test root file discovery by traversing to hierarchy root."""
        construct = self.create_test_construct(self.file)
        
        result = construct.get_root_file()
        
        self.assertEqual(result, self.file)

if __name__ == '__main__':
    unittest.main()