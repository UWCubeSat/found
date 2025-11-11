import unittest
from src.common.parsing.pemdas import get_lowest_operation


class TestGetLowestOperation(unittest.TestCase):
    """Test class for get_lowest_operation function"""
    
    def test_empty_expression(self):
        """Test empty expression"""
        result = get_lowest_operation("")
        assert result == (None, 0)

    def test_single_character(self):
        """Test single character (no operators)"""
        result = get_lowest_operation("a")
        assert result == (None, 0)

    def test_only_parentheses(self):
        """Test expression with only parentheses"""
        result = get_lowest_operation("()")
        assert result == (None, 0)
    
    def test_no_operators_in_parentheses(self):
        """Test expression where all operators are in parentheses"""
        result = get_lowest_operation("(b+a)")
        assert result == (None, 0)

    def test_simple_addition(self):
        """Test basic addition operation"""
        result = get_lowest_operation("a+b")
        assert result == ('+', 1)

    def test_simple_multiplication(self):
        """Test basic multiplication operation"""
        result = get_lowest_operation("a*b")
        assert result == ('*', 1)

    def test_precedence_order(self):
        """Test that lower precedence operations are returned"""
        result = get_lowest_operation("a+b*c")
        assert result == ('+', 1)

    def test_multiple_same_precedence(self):
        """Test multiple operations with same precedence returns rightmost"""
        result = get_lowest_operation("a+b-c")
        assert result == ('-', 3)

    def test_two_character_operators(self):
        """Test two-character operators like ==, !=, etc."""
        result = get_lowest_operation("a==b")
        assert result == ('==', 1)

    def test_mixed_operators(self):
        """Test mix of single and double character operators"""
        result = get_lowest_operation("a<=b+c")
        assert result == ('<=', 1)
    
    def test_unary_operators(self):
        """Test unary operators"""
        result = get_lowest_operation("!a+b")
        assert result == ('+', 2)

    def test_bitwise_operators(self):
        """Test bitwise operations"""
        result = get_lowest_operation("a&b|c")
        assert result == ('|', 3)

    def test_shift_operators(self):
        """Test shift operations"""
        result = get_lowest_operation("a<<b+c")
        assert result == ('<<', 1)

    def test_comparison_operators(self):
        """Test comparison operations"""
        result = get_lowest_operation("a<b+c")
        assert result == ('<', 1)

    def test_logical_operators(self):
        """Test logical operations"""
        result = get_lowest_operation("a&&b||c")
        assert result == ('||', 4)

    def test_parentheses_ignored(self):
        """Test operations inside parentheses are ignored"""
        result = get_lowest_operation("a+(b*c)")
        assert result == ('+', 1)

    def test_nested_parentheses(self):
        """Test nested parentheses"""
        result = get_lowest_operation("a+((b*c)+d)")
        assert result == ('+', 1)

    def test_multiple_delimiters(self):
        """Test different types of delimiters"""
        result = get_lowest_operation("a+{b*[c+d]}")
        assert result == ('+', 1)

    def test_complex_expression(self):
        """Test complex expression with multiple precedence levels"""
        result = get_lowest_operation("a||b&&c|d^e&f==g<h<<i+j*k")
        assert result == ('||', 1)

    def test_consecutive_operators(self):
        """Test consecutive operators"""
        result = get_lowest_operation("a+b-c")
        assert result == ('-', 3)

    def test_all_precedence_levels(self):
        """Test expression with all precedence levels"""
        expr = "a||b&&c|d^e&f!=g<h<<i+j*k"
        result = get_lowest_operation(expr)
        assert result == ('||', 1)

    def test_rightmost_same_precedence(self):
        """Test that rightmost operator of same precedence is chosen"""
        result = get_lowest_operation("a*b/c%d")
        assert result == ('%', 5)