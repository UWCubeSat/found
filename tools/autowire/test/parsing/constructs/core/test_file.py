"""Tests for file construct class.

This module tests the File construct functionality including:
- File initialization with path storage
- Construct addition and categorization
- Filtered construct collections management
- Type-based construct retrieval
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.classes import Class
from src.parsing.constructs.definitions.functions import Function
from src.parsing.constructs.types.types import Type
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, CLASS_NAME, FUNCTION_NAME, INT_TYPE,
    LINE_NUMBER_10, LINE_NUMBER_15, LINE_NUMBER_20,
    EXPECTED_COUNT_0, EXPECTED_COUNT_1, EXPECTED_COUNT_2
)


class TestFile(unittest.TestCase):
    """Test cases for File construct functionality.
    
    Tests file-level operations including construct storage,
    categorization, and retrieval by type.
    """
    
    def setUp(self):
        """Set up test fixtures with empty file."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_test_class(self):
        """Create a test class construct.
        
        Returns:
            Class: Test class with standard name
        """
        return Class(CLASS_NAME)
    
    def create_test_function(self):
        """Create a test function construct.
        
        Returns:
            Function: Test function with int return type
        """
        return_type = Type(INT_TYPE)
        return Function(FUNCTION_NAME, return_type)
    
    def test_file_initialization(self):
        """Test file initialization stores path and creates empty collections."""
        file = File(SAMPLE_FILE_PATH)
        
        expected = {
            'parent': None,
            'name': SAMPLE_FILE_PATH,
            'comments': [],
            'constructs': [],
            'classes': [],
            'functions': [],
            'constructors': [],
            'destructors': [],
            'variables': [],
            'enums': [],
            'namespaces': [],
            'typedefs': [],
            'using_declarations': [],
            'includes': [],
            'macros': [],
            'all_comments': [],
            'comment_associations': {}
        }
        
        self.assertEqual(expected, file.__dict__)
    
    def test_add_construct_class(self):
        """Test adding class construct updates collections and sets parent."""
        test_class = self.create_test_class()
        
        self.file.add_construct(test_class, LINE_NUMBER_10)
        expected_result = {
            'constructs_count': EXPECTED_COUNT_1,
            'classes_count': EXPECTED_COUNT_1,
            'class_entry': (test_class, LINE_NUMBER_10),
            'parent_set': self.file
        }
        actual_result = {
            'constructs_count': len(self.file.constructs),
            'classes_count': len(self.file.classes),
            'class_entry': self.file.classes[0],
            'parent_set': test_class.parent
        }
        
        self.assertEqual(actual_result, expected_result)
    
    def test_add_construct_function(self):
        """Test adding function construct updates appropriate collections."""
        test_function = self.create_test_function()
        
        self.file.add_construct(test_function, LINE_NUMBER_15)
        expected_result = {
            'constructs_count': EXPECTED_COUNT_1,
            'functions_count': EXPECTED_COUNT_1,
            'function_entry': (test_function, LINE_NUMBER_15)
        }
        actual_result = {
            'constructs_count': len(self.file.constructs),
            'functions_count': len(self.file.functions),
            'function_entry': self.file.functions[0]
        }
        
        self.assertEqual(actual_result, expected_result)
    
    def test_get_constructs_by_type(self):
        """Test type-based filtering returns correct construct collections."""
        test_class = self.create_test_class()
        test_function = self.create_test_function()
        self.file.add_construct(test_class, LINE_NUMBER_10)
        self.file.add_construct(test_function, LINE_NUMBER_20)
        
        class_results = self.file.get_constructs_by_type(Class)
        function_results = self.file.get_constructs_by_type(Function)
        expected_results = {
            'class_count': EXPECTED_COUNT_1,
            'function_count': EXPECTED_COUNT_1,
            'class_construct': test_class,
            'function_construct': test_function
        }
        actual_results = {
            'class_count': len(class_results),
            'function_count': len(function_results),
            'class_construct': class_results[0][0],
            'function_construct': function_results[0][0]
        }
        
        self.assertEqual(actual_results, expected_results)
