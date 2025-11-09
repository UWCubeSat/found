"""Tests for type construct classes.

This module tests Type and Value construct functionality including:
- Type initialization with qualifiers and template arguments
- Raw pointer level handling and smart pointer representation
- Type property queries (is_pointer, get_pointer_level, get_base_type)
- Value expression storage and retrieval
"""

import unittest
from src.parsing.constructs.types.types import Type, Value
from src.parsing.constructs.core.file import File
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, INT_TYPE, STRING_TYPE, VECTOR_TYPE, UNIQUE_PTR_TYPE,
    SIMPLE_EXPRESSION, FUNCTION_CALL_EXPRESSION, POINTER_LEVEL_1, POINTER_LEVEL_2, POINTER_LEVEL_3,
    EXPECTED_COUNT_0, EXPECTED_COUNT_1, EXPECTED_TRUE, EXPECTED_FALSE
)


class TestType(unittest.TestCase):
    """Test cases for Type construct functionality.
    
    Tests type representation including raw pointers, smart pointers,
    template arguments, and type property queries.
    """
    
    def setUp(self):
        """Set up test fixtures with file context."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_type(self, type_name=INT_TYPE):
        """Create a simple non-pointer type.
        
        Args:
            type_name: Name of the type to create
            
        Returns:
            Type: Simple type construct
        """
        return Type(self.file, type_name)
    
    def create_pointer_type(self, type_name=INT_TYPE, level=POINTER_LEVEL_1):
        """Create a raw pointer type with specified indirection level.
        
        Args:
            type_name: Base type name
            level: Number of pointer indirection levels
            
        Returns:
            Type: Pointer type construct
        """
        return Type(self.file, type_name, raw_pointer_level=level)
    
    def create_template_type(self, type_name=VECTOR_TYPE, arg_type=INT_TYPE):
        """Create a template type with single type argument.
        
        Args:
            type_name: Template type name
            arg_type: Template argument type name
            
        Returns:
            Type: Template type construct
        """
        template_arg = Type(self.file, arg_type)
        return Type(self.file, type_name, template_args=[template_arg])
    
    def test_type_initialization_simple(self):
        """Test simple type initialization sets all properties correctly."""
        type_obj = self.create_simple_type()
        expected_properties = {
            'qualified_name': INT_TYPE,
            'raw_pointer_level': EXPECTED_COUNT_0,
            'is_reference': EXPECTED_FALSE,
            'is_const': EXPECTED_FALSE,
            'template_args_count': EXPECTED_COUNT_0
        }
        actual_properties = {
            'qualified_name': type_obj.qualified_name,
            'raw_pointer_level': type_obj.raw_pointer_level,
            'is_reference': type_obj.is_reference,
            'is_const': type_obj.is_const,
            'template_args_count': len(type_obj.template_args)
        }
        
        self.assertEqual(actual_properties, expected_properties)
    
    def test_type_initialization_pointer(self):
        """Test pointer type initialization sets pointer properties correctly."""
        type_obj = self.create_pointer_type(level=POINTER_LEVEL_2)
        expected_properties = {
            'qualified_name': INT_TYPE,
            'raw_pointer_level': POINTER_LEVEL_2,
            'is_pointer': EXPECTED_TRUE
        }
        actual_properties = {
            'qualified_name': type_obj.qualified_name,
            'raw_pointer_level': type_obj.raw_pointer_level,
            'is_pointer': type_obj.is_pointer
        }
        
        self.assertEqual(actual_properties, expected_properties)
    
    def test_is_pointer_true(self):
        """Test is_pointer property correctly identifies pointer types."""
        type_obj = self.create_pointer_type()
        
        self.assertEqual(type_obj.is_pointer, EXPECTED_TRUE)
    
    def test_is_pointer_false(self):
        """Test is_pointer property correctly identifies non-pointer types."""
        type_obj = self.create_simple_type()
        
        self.assertEqual(type_obj.is_pointer, EXPECTED_FALSE)
    
    def test_get_pointer_level(self):
        """Test get_pointer_level returns correct indirection depth."""
        type_obj = self.create_pointer_type(level=POINTER_LEVEL_3)
        
        result = type_obj.get_pointer_level()
        
        self.assertEqual(result, POINTER_LEVEL_3)
    
    def test_get_base_type_pointer(self):
        """Test get_base_type removes pointer indirection correctly."""
        type_obj = self.create_pointer_type(level=POINTER_LEVEL_2)
        
        result = type_obj.get_base_type()
        expected_base_properties = {
            'qualified_name': INT_TYPE,
            'raw_pointer_level': EXPECTED_COUNT_0,
            'is_pointer': EXPECTED_FALSE
        }
        actual_base_properties = {
            'qualified_name': result.qualified_name,
            'raw_pointer_level': result.raw_pointer_level,
            'is_pointer': result.is_pointer
        }
        
        self.assertEqual(actual_base_properties, expected_base_properties)
    
    def test_smart_pointer_usage(self):
        """Test smart pointer representation uses template args not raw pointer level."""
        inner_type = Type(self.file, INT_TYPE)
        smart_ptr = Type(self.file, UNIQUE_PTR_TYPE, template_args=[inner_type])
        
        expected_smart_ptr_properties = {
            'qualified_name': UNIQUE_PTR_TYPE,
            'template_args_count': EXPECTED_COUNT_1,
            'inner_type_name': INT_TYPE,
            'is_pointer': EXPECTED_FALSE  # Smart pointers are not raw pointers
        }
        actual_smart_ptr_properties = {
            'qualified_name': smart_ptr.qualified_name,
            'template_args_count': len(smart_ptr.template_args),
            'inner_type_name': smart_ptr.template_args[0].qualified_name,
            'is_pointer': smart_ptr.is_pointer
        }
        
        self.assertEqual(actual_smart_ptr_properties, expected_smart_ptr_properties)


class TestValue(unittest.TestCase):
    """Test cases for Value construct functionality.
    
    Tests value expression storage for literals, function calls,
    and other C++ expressions.
    """
    
    def setUp(self):
        """Set up test fixtures with file context."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_value(self, expression=SIMPLE_EXPRESSION):
        """Create a value with specified expression.
        
        Args:
            expression: Expression string to store
            
        Returns:
            Value: Value construct with expression
        """
        return Value(self.file, expression)
    
    def test_value_initialization_literal(self):
        """Test value initialization stores expression and parent correctly."""
        value = self.create_simple_value()
        
        expected = {
            'parent': self.file,
            'comments': [],
            'expression': SIMPLE_EXPRESSION
        }
        
        self.assertEqual(expected, value.__dict__)
    
    def test_value_initialization_function_call(self):
        """Test value initialization with function call expression."""
        value = Value(self.file, FUNCTION_CALL_EXPRESSION)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'expression': FUNCTION_CALL_EXPRESSION
        }
        
        self.assertEqual(expected, value.__dict__)


if __name__ == '__main__':
    unittest.main()