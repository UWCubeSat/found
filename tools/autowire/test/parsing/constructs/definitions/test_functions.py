"""Tests for function construct classes.

This module tests Function and Parameter constructs which represent C++ function
declarations and their parameters. Tests cover initialization with various
modifiers, template parameters, and parameter configurations.
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.functions import Function, Parameter
from src.parsing.constructs.types.types import Type, Value
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, FUNCTION_NAME, INT_TYPE, SIMPLE_EXPRESSION, TEMPLATE_PARAM_T
)


class TestFunction(unittest.TestCase):
    """Test cases for Function construct.
    
    Tests function initialization with different modifiers including virtual,
    static, const, pure virtual, template parameters, and parameter lists.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.return_type = Type(self.file, INT_TYPE)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_function(self, name=FUNCTION_NAME):
        """Helper to create a simple function with default parameters.
        
        Args:
            name (str): Function name, defaults to FUNCTION_NAME
            
        Returns:
            Function: Simple function with int return type
        """
        return Function(self.file, name, self.return_type)
    
    def create_parameter(self, name="param", type_name=INT_TYPE):
        """Helper to create a parameter with specified type.
        
        Args:
            name (str): Parameter name, defaults to "param"
            type_name (str): Type name, defaults to INT_TYPE
            
        Returns:
            Parameter: Parameter with specified name and type
        """
        param_type = Type(self.file, type_name)
        return Parameter(self.file, name, param_type)
    
    def test_function_initialization_simple(self):
        """Test simple function initialization with default parameters."""
        func = self.create_simple_function()
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': [],
            'is_virtual': False,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': False,
            'template_parameters': [],
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)
    
    def test_function_initialization_virtual(self):
        """Test virtual function initialization with is_virtual=True."""
        func = Function(self.file, FUNCTION_NAME, self.return_type, is_virtual=True)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': [],
            'is_virtual': True,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': False,
            'template_parameters': [],
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)
    
    def test_function_initialization_pure_virtual(self):
        """Test pure virtual function initialization with is_pure_virtual=True."""
        func = Function(self.file, FUNCTION_NAME, self.return_type, is_pure_virtual=True)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': [],
            'is_virtual': False,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': True,
            'template_parameters': [],
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)
    
    def test_function_with_parameters(self):
        """Test function with parameter list."""
        param = self.create_parameter()
        parameters = [param]
        func = Function(self.file, FUNCTION_NAME, self.return_type, parameters=parameters)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': parameters,
            'is_virtual': False,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': False,
            'template_parameters': [],
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)
    
    def test_function_with_multiple_parameters(self):
        """Test function with multiple parameters."""
        param1 = self.create_parameter("x", INT_TYPE)
        param2 = self.create_parameter("y", "double")
        param3 = self.create_parameter("name", "string")
        parameters = [param1, param2, param3]
        func = Function(self.file, FUNCTION_NAME, self.return_type, parameters=parameters)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': parameters,
            'is_virtual': False,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': False,
            'template_parameters': [],
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)
    
    def test_function_template(self):
        """Test template function with template parameters."""
        template_params = [TEMPLATE_PARAM_T]
        func = Function(self.file, FUNCTION_NAME, self.return_type, template_parameters=template_params)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': FUNCTION_NAME,
            'return_type': self.return_type,
            'parameters': [],
            'is_virtual': False,
            'is_static': False,
            'is_const': False,
            'is_pure_virtual': False,
            'template_parameters': template_params,
            'body': [],
            'comment': None,
            'namespace': None
        }
        
        self.assertEqual(expected, func.__dict__)


class TestParameter(unittest.TestCase):
    """Test cases for Parameter construct.
    
    Tests parameter initialization with different configurations including
    default values and various parameter types.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.param_type = Type(self.file, INT_TYPE)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_parameter(self, name="param"):
        """Helper to create a simple parameter with default type.
        
        Args:
            name (str): Parameter name, defaults to "param"
            
        Returns:
            Parameter: Simple parameter with int type
        """
        return Parameter(self.file, name, self.param_type)
    
    def test_parameter_initialization_simple(self):
        """Test simple parameter initialization with default values."""
        param = self.create_simple_parameter()
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': "param",
            'type': self.param_type,
            'default_value': None
        }
        
        self.assertEqual(expected, param.__dict__)
    
    def test_parameter_with_default_value(self):
        """Test parameter with specified default value."""
        default_val = Value(self.file, SIMPLE_EXPRESSION)
        param = Parameter(self.file, "param", self.param_type, default_value=default_val)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': "param",
            'type': self.param_type,
            'default_value': default_val
        }
        
        self.assertEqual(expected, param.__dict__)


if __name__ == '__main__':
    unittest.main()