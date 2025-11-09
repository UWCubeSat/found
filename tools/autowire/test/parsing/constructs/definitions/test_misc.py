"""Tests for miscellaneous construct classes.

This module tests various utility constructs including Namespace, Typedef, Using,
Include, Macro, and Comment. These constructs represent common C++ language
features and preprocessor directives.
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.misc import Namespace, Typedef, Using, Include, Macro, Comment
from src.parsing.constructs.core.base import Definition
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, NAMESPACE_NAME, TYPEDEF_NAME, MACRO_NAME, STRING_TYPE,
    SINGLE_LINE_COMMENT, MULTI_LINE_COMMENT, MACRO_DEFINITION, MACRO_WITH_PARAMS,
    USING_NAMESPACE, USING_TYPE, HEADER_FILE_PATH
, set_parent)


class TestNamespace(unittest.TestCase):
    """Test cases for Namespace construct.
    
    Tests namespace declaration initialization and basic properties.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_namespace_initialization(self):
        """Test namespace initialization with name and parent."""
        namespace = set_parent(Namespace(NAMESPACE_NAME), self.file)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'name': NAMESPACE_NAME
        }
        
        self.assertEqual(expected, namespace.__dict__)


class TestTypedef(unittest.TestCase):
    """Test cases for Typedef construct.
    
    Tests typedef declaration initialization with alias names and original types.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_typedef_initialization(self):
        """Test typedef initialization with alias name and original type."""
        original_type = set_parent(Definition(STRING_TYPE), self.file)
        typedef = Typedef(TYPEDEF_NAME, original_type)
        
        expected = {
            'parent': None,
            'comments': [],
            'name': TYPEDEF_NAME,
            'original_type': original_type
        }
        
        self.assertEqual(expected, typedef.__dict__)


class TestUsing(unittest.TestCase):
    """Test cases for Using construct.
    
    Tests using declarations and using directives with different targets
    and directive flags.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_using_declaration(self):
        """Test using declaration initialization with is_directive=False."""
        using = Using(USING_TYPE, is_directive=False)
        
        expected = {
            'parent': None,
            'comments': [],
            'target': USING_TYPE,
            'is_directive': False
        }
        
        self.assertEqual(expected, using.__dict__)
    
    def test_using_directive(self):
        """Test using directive initialization with is_directive=True."""
        using = Using(USING_NAMESPACE, is_directive=True)
        
        expected = {
            'parent': None,
            'comments': [],
            'target': USING_NAMESPACE,
            'is_directive': True
        }
        
        self.assertEqual(expected, using.__dict__)


class TestInclude(unittest.TestCase):
    """Test cases for Include construct.
    
    Tests include directive initialization with local and system includes.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.header_file = File(HEADER_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_include_initialization_local(self):
        """Test local include initialization with is_system=False."""
        include = Include(self.header_file, is_system=False)
        
        expected = {
            'parent': None,
            'comments': [],
            'path': self.header_file,
            'is_system': False
        }
        
        self.assertEqual(expected, include.__dict__)
    
    def test_include_initialization_system(self):
        """Test system include initialization with is_system=True."""
        include = Include(self.header_file, is_system=True)
        
        expected = {
            'parent': None,
            'comments': [],
            'path': self.header_file,
            'is_system': True
        }
        
        self.assertEqual(expected, include.__dict__)


class TestMacro(unittest.TestCase):
    """Test cases for Macro construct.
    
    Tests macro definition initialization with various parameter configurations
    and definition strings.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_macro_simple(self):
        """Test simple macro initialization without parameters."""
        macro = Macro(MACRO_NAME, MACRO_DEFINITION)
        
        expected = {
            'parent': None,
            'comments': [],
            'name': MACRO_NAME,
            'definition': MACRO_DEFINITION,
            'parameters': []
        }
        
        self.assertEqual(expected, macro.__dict__)
    
    def test_macro_with_parameters(self):
        """Test macro with parameter list initialization."""
        macro = Macro(MACRO_NAME, MACRO_DEFINITION, MACRO_WITH_PARAMS)
        
        expected = {
            'parent': None,
            'comments': [],
            'name': MACRO_NAME,
            'definition': MACRO_DEFINITION,
            'parameters': MACRO_WITH_PARAMS
        }
        
        self.assertEqual(expected, macro.__dict__)
    
    def test_macro_no_definition(self):
        """Test macro without definition string."""
        macro = Macro(MACRO_NAME)
        
        expected = {
            'parent': None,
            'comments': [],
            'name': MACRO_NAME,
            'definition': "",
            'parameters': []
        }
        
        self.assertEqual(expected, macro.__dict__)


class TestComment(unittest.TestCase):
    """Test cases for Comment construct.
    
    Tests comment initialization with single-line and multi-line configurations.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_comment_single_line(self):
        """Test single line comment initialization with default multiline=False."""
        comment = set_parent(Comment(SINGLE_LINE_COMMENT), self.file)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'text': SINGLE_LINE_COMMENT,
            'is_multiline': False
        }
        
        self.assertEqual(expected, comment.__dict__)
    
    def test_comment_multi_line(self):
        """Test multi line comment initialization with is_multiline=True."""
        comment = Comment(MULTI_LINE_COMMENT, is_multiline=True)
        
        expected = {
            'parent': None,
            'comments': [],
            'text': MULTI_LINE_COMMENT,
            'is_multiline': True
        }
        
        self.assertEqual(expected, comment.__dict__)


if __name__ == '__main__':
    unittest.main()