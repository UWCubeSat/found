"""Tests for variable construct classes.

This module tests the Variable construct which represents C++ variable
declarations with various modifiers including static, const, mutable,
initial values, and associated comments.
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.statements.variables import Variable
from src.parsing.constructs.types.types import Type, Value
from src.parsing.constructs.definitions.misc import Comment
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, VARIABLE_NAME, INT_TYPE, SIMPLE_EXPRESSION, SINGLE_LINE_COMMENT
, set_parent)


class TestVariable(unittest.TestCase):
    """Test cases for Variable construct.
    
    Tests variable initialization with different modifiers, initial values,
    and comment associations.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.var_type = set_parent(Type(INT_TYPE), self.file)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_variable(self, name=VARIABLE_NAME):
        """Helper to create a simple variable with default type.
        
        Args:
            name (str): Variable name, defaults to VARIABLE_NAME
            
        Returns:
            Variable: Simple variable with int type
        """
        return set_parent(Variable(name, self.var_type), self.file)
    
    def test_variable_initialization_simple(self):
        """Test simple variable initialization with default parameters."""
        var = self.create_simple_variable()
        
        expected = {
            'parent': self.file,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': None,
            'is_static': False,
            'is_const': False,
            'is_mutable': False,
            'comments': [],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_with_initial_value(self):
        """Test variable with specified initial value."""
        initial_val = set_parent(Value(SIMPLE_EXPRESSION), self.file)
        var = Variable(VARIABLE_NAME, self.var_type, initial_value=initial_val)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': initial_val,
            'is_static': False,
            'is_const': False,
            'is_mutable': False,
            'comments': [],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_static(self):
        """Test static variable with is_static=True."""
        var = Variable(VARIABLE_NAME, self.var_type, is_static=True)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': None,
            'is_static': True,
            'is_const': False,
            'is_mutable': False,
            'comments': [],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_const(self):
        """Test const variable with is_const=True."""
        var = Variable(VARIABLE_NAME, self.var_type, is_const=True)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': None,
            'is_static': False,
            'is_const': True,
            'is_mutable': False,
            'comments': [],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_mutable(self):
        """Test mutable variable with is_mutable=True."""
        var = Variable(VARIABLE_NAME, self.var_type, is_mutable=True)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': None,
            'is_static': False,
            'is_const': False,
            'is_mutable': True,
            'comments': [],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_with_comment(self):
        """Test variable with associated comment using setter."""
        comment = set_parent(Comment(SINGLE_LINE_COMMENT), self.file)
        var = Variable(VARIABLE_NAME, self.var_type)
        var.add_comment(comment)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': None,
            'is_static': False,
            'is_const': False,
            'is_mutable': False,
            'comments': [comment],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
    
    def test_variable_all_modifiers(self):
        """Test variable with all modifiers and associated comment."""
        initial_val = set_parent(Value(SIMPLE_EXPRESSION), self.file)
        comment = set_parent(Comment(SINGLE_LINE_COMMENT), self.file)
        
        var = Variable(
            VARIABLE_NAME, self.var_type,
            initial_value=initial_val, is_static=True, is_const=True,
            is_mutable=True
        )
        var.add_comment(comment)
        
        expected = {
            'parent': None,
            'name': VARIABLE_NAME,
            'type': self.var_type,
            'initial_value': initial_val,
            'is_static': True,
            'is_const': True,
            'is_mutable': True,
            'comments': [comment],
            'namespace': None
        }
        
        self.assertEqual(expected, var.__dict__)
