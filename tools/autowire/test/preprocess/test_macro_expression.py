import unittest
from src.preprocess.constructs.macros import MacroExpression, DefineDirective

# Define directives
ZERO_MACRO = DefineDirective("ZERO", "0")
ONE_MACRO = DefineDirective("ONE", "1")
TWO_MACRO = DefineDirective("TWO", "2")
THREE_MACRO = DefineDirective("THREE", "3")
FOUR_MACRO = DefineDirective("FOUR", "4")
FIVE_MACRO = DefineDirective("FIVE", "5")
SIX_MACRO = DefineDirective("SIX", "6")
SEVEN_MACRO = DefineDirective("SEVEN", "7")
EIGHT_MACRO = DefineDirective("EIGHT", "8")
TWELVE_MACRO = DefineDirective("TWELVE", "12")
DEFINED_MACRO = DefineDirective("DEFINED", "1")
EXISTS_MACRO = DefineDirective("EXISTS", "1")
TRUE_MACRO = DefineDirective("TRUE", "1")
FALSE_MACRO = DefineDirective("FALSE", "0")
A_MACRO = DefineDirective("A", "1")
B_MACRO = DefineDirective("B", "2")
A_MACRO_2 = DefineDirective("A", "2")
B_MACRO_1 = DefineDirective("B", "1")
A_RECURSIVE = DefineDirective("A", "B")
B_RECURSIVE = DefineDirective("B", "5")


class TestMacroExpression(unittest.TestCase):
    """Test class for MacroExpression.evaluate function"""

    def test_undefined_identifier_false(self):
        """Test undefined identifier returns False"""
        result = MacroExpression.evaluate("UNDEFINED", {})
        self.assertFalse(result)

    def test_defined_identifier_true(self):
        """Test defined identifier returns its value"""
        macros = {"DEFINED": DEFINED_MACRO}
        result = MacroExpression.evaluate("DEFINED", macros)
        self.assertEqual(result, 1)

    def test_zero_macro_false(self):
        """Test macro with value 0 returns 0 (falsy)"""
        macros = {"ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("ZERO", macros)
        self.assertEqual(result, 0)

    def test_nonzero_macro_true(self):
        """Test macro with non-zero value returns that value"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("FIVE", macros)
        self.assertEqual(result, 5)

    def test_defined_function_undefined_false(self):
        """Test defined() with undefined macro returns False"""
        result = MacroExpression.evaluate("defined(UNDEFINED)", {})
        self.assertFalse(result)

    def test_defined_function_defined_true(self):
        """Test defined() with defined macro returns True"""
        macros = {"EXISTS": EXISTS_MACRO}
        result = MacroExpression.evaluate("defined(EXISTS)", macros)
        self.assertTrue(result)

    def test_logical_not_true_false(self):
        """Test !1 returns False"""
        macros = {"TRUE": TRUE_MACRO}
        result = MacroExpression.evaluate("!TRUE", macros)
        self.assertFalse(result)

    def test_logical_not_false_true(self):
        """Test !0 returns True"""
        macros = {"FALSE": FALSE_MACRO}
        result = MacroExpression.evaluate("!FALSE", macros)
        self.assertTrue(result)

    def test_bitwise_not_zero(self):
        """Test ~0 returns -1"""
        macros = {"ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("~ZERO", macros)
        self.assertEqual(result, -1)

    def test_bitwise_not_one(self):
        """Test ~1 returns -2"""
        macros = {"ONE": ONE_MACRO}
        result = MacroExpression.evaluate("~ONE", macros)
        self.assertEqual(result, -2)

    def test_parentheses_grouping(self):
        """Test parentheses preserve evaluation"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("(FIVE)", macros)
        self.assertEqual(result, 5)

    def test_addition_true(self):
        """Test 1 + 1 = 2"""
        macros = {"ONE": ONE_MACRO}
        result = MacroExpression.evaluate("ONE + ONE", macros)
        self.assertEqual(result, 2)

    def test_addition_false(self):
        """Test 0 + 0 = 0"""
        macros = {"ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("ZERO + ZERO", macros)
        self.assertEqual(result, 0)

    def test_subtraction_positive(self):
        """Test 5 - 3 = 2"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE - THREE", macros)
        self.assertEqual(result, 2)

    def test_subtraction_zero(self):
        """Test 3 - 3 = 0"""
        macros = {"THREE": THREE_MACRO}
        result = MacroExpression.evaluate("THREE - THREE", macros)
        self.assertEqual(result, 0)

    def test_multiplication_nonzero(self):
        """Test 3 * 4 = 12"""
        macros = {"THREE": THREE_MACRO, "FOUR": FOUR_MACRO}
        result = MacroExpression.evaluate("THREE * FOUR", macros)
        self.assertEqual(result, 12)

    def test_multiplication_zero(self):
        """Test 5 * 0 = 0"""
        macros = {"FIVE": FIVE_MACRO, "ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("FIVE * ZERO", macros)
        self.assertEqual(result, 0)

    def test_division_nonzero(self):
        """Test 8 / 2 = 4"""
        macros = {"EIGHT": EIGHT_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("EIGHT / TWO", macros)
        self.assertEqual(result, 4)

    def test_division_one(self):
        """Test 7 / 7 = 1"""
        macros = {"SEVEN": SEVEN_MACRO}
        result = MacroExpression.evaluate("SEVEN / SEVEN", macros)
        self.assertEqual(result, 1)

    def test_modulo_nonzero(self):
        """Test 7 % 3 = 1"""
        macros = {"SEVEN": SEVEN_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("SEVEN % THREE", macros)
        self.assertEqual(result, 1)

    def test_modulo_zero(self):
        """Test 6 % 3 = 0"""
        macros = {"SIX": SIX_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("SIX % THREE", macros)
        self.assertEqual(result, 0)

    def test_equality_true(self):
        """Test 5 == 5 returns True"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("FIVE == FIVE", macros)
        self.assertEqual(result, 1)

    def test_equality_false(self):
        """Test 5 == 3 returns False"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE == THREE", macros)
        self.assertEqual(result, 0)

    def test_inequality_true(self):
        """Test 5 != 3 returns True"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE != THREE", macros)
        self.assertEqual(result, 1)

    def test_inequality_false(self):
        """Test 5 != 5 returns False"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("FIVE != FIVE", macros)
        self.assertEqual(result, 0)

    def test_less_than_true(self):
        """Test 3 < 5 returns True"""
        macros = {"THREE": THREE_MACRO, "FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("THREE < FIVE", macros)
        self.assertEqual(result, 1)

    def test_less_than_false(self):
        """Test 5 < 3 returns False"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE < THREE", macros)
        self.assertEqual(result, 0)

    def test_less_equal_true(self):
        """Test 3 <= 5 returns True"""
        macros = {"THREE": THREE_MACRO, "FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("THREE <= FIVE", macros)
        self.assertEqual(result, 1)

    def test_less_equal_false(self):
        """Test 5 <= 3 returns False"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE <= THREE", macros)
        self.assertEqual(result, 0)

    def test_greater_than_true(self):
        """Test 5 > 3 returns True"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE > THREE", macros)
        self.assertEqual(result, 1)

    def test_greater_than_false(self):
        """Test 3 > 5 returns False"""
        macros = {"THREE": THREE_MACRO, "FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("THREE > FIVE", macros)
        self.assertEqual(result, 0)

    def test_greater_equal_true(self):
        """Test 5 >= 3 returns True"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE >= THREE", macros)
        self.assertEqual(result, 1)

    def test_greater_equal_false(self):
        """Test 3 >= 5 returns False"""
        macros = {"THREE": THREE_MACRO, "FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("THREE >= FIVE", macros)
        self.assertEqual(result, 0)

    def test_bitwise_and_nonzero(self):
        """Test 5 & 3 = 1"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE & THREE", macros)
        self.assertEqual(result, 1)

    def test_bitwise_and_zero(self):
        """Test 5 & 2 = 0"""
        macros = {"FIVE": FIVE_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("FIVE & TWO", macros)
        self.assertEqual(result, 0)

    def test_bitwise_or_nonzero(self):
        """Test 5 | 3 = 7"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE | THREE", macros)
        self.assertEqual(result, 7)

    def test_bitwise_or_same(self):
        """Test 5 | 5 = 5"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("FIVE | FIVE", macros)
        self.assertEqual(result, 5)

    def test_bitwise_xor_nonzero(self):
        """Test 5 ^ 3 = 6"""
        macros = {"FIVE": FIVE_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("FIVE ^ THREE", macros)
        self.assertEqual(result, 6)

    def test_bitwise_xor_zero(self):
        """Test 5 ^ 5 = 0"""
        macros = {"FIVE": FIVE_MACRO}
        result = MacroExpression.evaluate("FIVE ^ FIVE", macros)
        self.assertEqual(result, 0)

    def test_left_shift_nonzero(self):
        """Test 3 << 2 = 12"""
        macros = {"THREE": THREE_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("THREE << TWO", macros)
        self.assertEqual(result, 12)

    def test_left_shift_zero(self):
        """Test 0 << 3 = 0"""
        macros = {"ZERO": ZERO_MACRO, "THREE": THREE_MACRO}
        result = MacroExpression.evaluate("ZERO << THREE", macros)
        self.assertEqual(result, 0)

    def test_right_shift_nonzero(self):
        """Test 12 >> 2 = 3"""
        macros = {"TWELVE": TWELVE_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("TWELVE >> TWO", macros)
        self.assertEqual(result, 3)

    def test_right_shift_zero(self):
        """Test 3 >> 3 = 0"""
        macros = {"THREE": THREE_MACRO}
        result = MacroExpression.evaluate("THREE >> THREE", macros)
        self.assertEqual(result, 0)

    def test_logical_and_true(self):
        """Test 1 && 1 returns True"""
        macros = {"ONE": ONE_MACRO}
        result = MacroExpression.evaluate("ONE && ONE", macros)
        self.assertEqual(result, 1)

    def test_logical_and_false(self):
        """Test 1 && 0 returns False"""
        macros = {"ONE": ONE_MACRO, "ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("ONE && ZERO", macros)
        self.assertEqual(result, 0)

    def test_logical_or_true(self):
        """Test 0 || 1 returns True"""
        macros = {"ZERO": ZERO_MACRO, "ONE": ONE_MACRO}
        result = MacroExpression.evaluate("ZERO || ONE", macros)
        self.assertEqual(result, 1)

    def test_logical_or_false(self):
        """Test 0 || 0 returns False"""
        macros = {"ZERO": ZERO_MACRO}
        result = MacroExpression.evaluate("ZERO || ZERO", macros)
        self.assertEqual(result, 0)

    def test_complex_expression_true(self):
        """Test complex expression that evaluates to True"""
        macros = {"A": A_MACRO, "B": B_MACRO}
        result = MacroExpression.evaluate("A && (B > A)", macros)
        self.assertEqual(result, 1)

    def test_complex_expression_false(self):
        """Test complex expression that evaluates to False"""
        macros = {"A": A_MACRO_2, "B": B_MACRO_1}
        result = MacroExpression.evaluate("A && (B > A)", macros)
        self.assertEqual(result, 0)

    def test_precedence_arithmetic_vs_comparison_true(self):
        """Test 1 + 2 > 2 returns True"""
        macros = {"ONE": ONE_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("ONE + TWO > TWO", macros)
        self.assertEqual(result, 1)

    def test_precedence_arithmetic_vs_comparison_false(self):
        """Test 1 + 1 > 2 returns False"""
        macros = {"ONE": ONE_MACRO, "TWO": TWO_MACRO}
        result = MacroExpression.evaluate("ONE + ONE > TWO", macros)
        self.assertEqual(result, 0)

    def test_recursive_macro_expansion(self):
        """Test recursive macro expansion"""
        macros = {"A": A_RECURSIVE, "B": B_RECURSIVE}
        result = MacroExpression.evaluate("A", macros)
        self.assertEqual(result, 5)