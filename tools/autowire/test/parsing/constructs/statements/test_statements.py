"""Tests for statement construct classes.

This module tests various statement constructs including base Statement class,
SimpleStatement, IfStatement, ForLoop, WhileLoop, and SwitchStatement.
These represent different C++ control flow and statement types.
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.statements.statements import (
    Statement, SimpleStatement, IfStatement, ForLoop, WhileLoop, SwitchStatement
)
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, CONDITION_EXPRESSION, LOOP_INIT, LOOP_CONDITION, LOOP_INCREMENT
, set_parent)


class TestStatement(unittest.TestCase):
    """Test cases for Statement base class.
    
    Tests the base Statement construct initialization and parent relationships.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_statement(self):
        """Helper to create a simple statement with file parent.
        
        Returns:
            Statement: Basic statement with file as parent
        """
        stmt = Statement(); stmt.parent = self.file; return stmt
    
    def test_statement_initialization(self):
        """Test statement initialization with parent file."""
        stmt = self.create_simple_statement()
        
        expected = {
            'parent': self.file,
            'comments': []
        }
        
        self.assertEqual(expected, stmt.__dict__)


class TestSimpleStatement(unittest.TestCase):
    """Test cases for SimpleStatement construct.
    
    Tests simple statement initialization with code strings and parent relationships.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_statement(self, code="x = 42;"):
        """Helper to create a simple statement with code.
        
        Args:
            code (str): Code string for the statement, defaults to "x = 42;"
            
        Returns:
            SimpleStatement: Statement with specified code
        """
        stmt = SimpleStatement(code); stmt.parent = self.file; return stmt
    
    def test_simple_statement_initialization(self):
        """Test simple statement initialization with code and parent."""
        code = "int x = 42;"
        stmt = self.create_simple_statement(code)
        
        expected = {
            'parent': self.file,
            'comments': [],
            'code': code
        }
        
        self.assertEqual(expected, stmt.__dict__)
    
    def test_simple_statement_assignment(self):
        """Test simple assignment statement with increment code."""
        code = "counter += 1;"
        stmt = SimpleStatement(code)
        
        expected = {
            'parent': None,
            'comments': [],
            'code': code
        }
        
        self.assertEqual(expected, stmt.__dict__)


class TestIfStatement(unittest.TestCase):
    """Test cases for IfStatement construct.
    
    Tests if statement initialization with conditions, then/else bodies,
    and else-if clauses.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_then_body(self):
        """Helper to create then body statements.
        
        Returns:
            List[SimpleStatement]: List containing return true statement
        """
        return [set_parent(SimpleStatement("return true;"), self.file)]
    
    def create_else_body(self):
        """Helper to create else body statements.
        
        Returns:
            List[SimpleStatement]: List containing return false statement
        """
        return [set_parent(SimpleStatement("return false;"), self.file)]
    
    def test_if_statement_simple(self):
        """Test simple if statement without else clause."""
        then_body = self.create_then_body()
        if_stmt = IfStatement(CONDITION_EXPRESSION, then_body)
        
        expected = {
            'condition': CONDITION_EXPRESSION,
            'then_body': then_body,
            'else_if_clauses_count': 0,
            'else_body': None
        }
        actual = {
            'condition': if_stmt.condition,
            'then_body': if_stmt.then_body,
            'else_if_clauses_count': len(if_stmt.else_if_clauses),
            'else_body': if_stmt.else_body
        }
        
        self.assertEqual(expected, actual)
    
    def test_if_statement_with_else(self):
        """Test if statement with else clause."""
        then_body = self.create_then_body()
        else_body = self.create_else_body()
        if_stmt = IfStatement(CONDITION_EXPRESSION, then_body, else_body=else_body)
        
        self.assertEqual(else_body, if_stmt.else_body)


class TestForLoop(unittest.TestCase):
    """Test cases for ForLoop construct.
    
    Tests for loop initialization with initialization, condition, increment,
    and body statements.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_loop_body(self):
        """Helper to create loop body statements.
        
        Returns:
            List[SimpleStatement]: List containing process call statement
        """
        return [set_parent(SimpleStatement("process(i);"), self.file)]
    
    def test_for_loop_initialization(self):
        """Test for loop initialization with all components."""
        body = self.create_loop_body()
        for_loop = ForLoop(LOOP_INIT, LOOP_CONDITION, LOOP_INCREMENT, body)
        
        expected = {
            'initialization': LOOP_INIT,
            'condition': LOOP_CONDITION,
            'increment': LOOP_INCREMENT,
            'body': body
        }
        actual = {
            'initialization': for_loop.initialization,
            'condition': for_loop.condition,
            'increment': for_loop.increment,
            'body': for_loop.body
        }
        
        self.assertEqual(expected, actual)


class TestWhileLoop(unittest.TestCase):
    """Test cases for WhileLoop construct.
    
    Tests while loop initialization with condition and body statements.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_loop_body(self):
        """Helper to create loop body statements.
        
        Returns:
            List[SimpleStatement]: List containing process call statement
        """
        return [set_parent(SimpleStatement("process();"), self.file)]
    
    def test_while_loop_initialization(self):
        """Test while loop initialization with condition and body."""
        body = self.create_loop_body()
        while_loop = WhileLoop(CONDITION_EXPRESSION, body)
        
        expected = {
            'condition': CONDITION_EXPRESSION,
            'body': body
        }
        actual = {
            'condition': while_loop.condition,
            'body': while_loop.body
        }
        
        self.assertEqual(expected, actual)


class TestSwitchStatement(unittest.TestCase):
    """Test cases for SwitchStatement construct.
    
    Tests switch statement initialization with expression, case clauses,
    and default cases.
    """
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_case_body(self, action="break;"):
        """Helper to create case body statements.
        
        Args:
            action (str): Action statement, defaults to "break;"
            
        Returns:
            List[SimpleStatement]: List containing action statement
        """
        return [SimpleStatement(action)]
    
    def test_switch_statement_simple(self):
        """Test simple switch statement with multiple cases."""
        case1_body = self.create_case_body("return 1;")
        case2_body = self.create_case_body("return 2;")
        cases = [("1", case1_body), ("2", case2_body)]
        switch_stmt = SwitchStatement("value", cases)
        
        expected = {
            'expression': "value",
            'cases_count': 2,
            'first_case_value': "1",
            'first_case_body': case1_body,
            'default_case': None
        }
        actual = {
            'expression': switch_stmt.expression,
            'cases_count': len(switch_stmt.cases),
            'first_case_value': switch_stmt.cases[0][0],
            'first_case_body': switch_stmt.cases[0][1],
            'default_case': switch_stmt.default_case
        }
        
        self.assertEqual(expected, actual)
