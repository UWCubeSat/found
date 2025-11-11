from typing import Set, Tuple

"""C++ preprocessor expression evaluation with operator precedence."""

from typing import Set, Tuple

# Operator precedence table (same as C preprocessor)
# Lower numbers = lower precedence (evaluated last)
PRECEDENCE = {
    '||': 1,   # Logical OR (lowest precedence)
    '&&': 2,   # Logical AND
    '|': 3,    # Bitwise OR
    '^': 4,    # Bitwise XOR
    '&': 5,    # Bitwise AND
    '==': 6, '!=': 6,  # Equality operators
    '<': 7, '<=': 7, '>': 7, '>=': 7,  # Relational operators
    '<<': 8, '>>': 8,  # Shift operators
    '+': 9, '-': 9,    # Addition/subtraction
    '*': 10, '/': 10, '%': 10,  # Multiplication/division/modulo (highest precedence)
    None: float('inf')  # No operation found
}

# Operation implementations for macro expression evaluation
OPERATIONS = {
    '||': lambda x, y: x or y,
    '&&': lambda x, y: x and y,
    '|': lambda x, y: x | y,
    '^': lambda x, y: x ^ y,
    '&': lambda x, y: x & y,
    '==': lambda x, y: x == y, '!=': lambda x, y: x != y,
    '<': lambda x, y: x < y, '<=': lambda x, y: x <= y, '>': lambda x, y: x > y, '>=': lambda x, y: x >= y,
    '<<': lambda x, y: x << y, '>>': lambda x, y: x >> y,
    '+': lambda x, y: x + y, '-': lambda x, y: x - y,
    '*': lambda x, y: x * y, '/': lambda x, y: x // y, '%': lambda x, y: x % y,
    '!': lambda x: not x, '~': lambda x: ~x,  # unary operators
}

def get_lowest_operation(expression: str,
                         open_deliminers: Set[str] = {'(', '{', '['},
                         close_deliminers: Set[str] = {')', '}', ']'}) -> Tuple[str, int]:
    """Find the lowest precedence operation in an expression for parsing.

    Args:
        expression (str): The expression to analyze
        open_deliminers (Set[str]): Opening delimiter characters
        close_deliminers (Set[str]): Closing delimiter characters
        
    Returns:
        Tuple[str, int]: (operator, position) of the lowest precedence operation,
                        or (None, 0) if no operations found
                        
    Note:
        Uses C preprocessor operator precedence rules. Operations inside
        delimiters are ignored. Returns rightmost operation for same precedence.
    """
    depth = 0
    i = 0
    operation = None
    operation_index = 0
    while i != len(expression):
        char = expression[i]
        if char in open_deliminers:
            depth += 1
        elif char in close_deliminers:
            depth -= 1
        else:
            if i != len(expression) - 1:
                word = expression[i:i+2]
                if word in PRECEDENCE and PRECEDENCE[word] <= PRECEDENCE[operation] and depth == 0:
                    operation = word
                    operation_index = i
                    i += 2
                    continue
            if char in PRECEDENCE and PRECEDENCE[char] <= PRECEDENCE[operation] and depth == 0:
                operation = char
                operation_index = i
        i += 1
    
    return operation, operation_index