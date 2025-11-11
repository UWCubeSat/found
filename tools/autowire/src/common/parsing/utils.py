"""Common parsing utilities."""

from typing import List, Tuple


def parse_delimited_list(text: str, delimiter: str = ',', open_char: str = '(', close_char: str = ')', level: int = 0) -> Tuple[List[str], Tuple[int, int]]:
    """Parse a delimited list, handling nested brackets/parentheses.
    
    Args:
        text: The text to parse (e.g., "arg1, func(a, b), arg3")
        delimiter: The delimiter character (default: ',')
        open_char: Opening bracket/parenthesis (default: '(')
        close_char: Closing bracket/parenthesis (default: ')')
    
    Returns:
        List of parsed elements with whitespace stripped, with the index range of this list
        including the borders if there is one
    
    Example:
        parse_delimited_list("arg1, func(a, b), arg3") 
        # Returns: ["arg1", "func(a, b)", "arg3"], (start_i, end_i)
    """
    if not text.strip():
        return [], (0, 0)
    
    elements = []
    current_element = ""
    depth = 0
    index = 0
    start = 0
    
    for char in text:
        if char == delimiter and depth == level:
            elements.append(current_element.strip())
            current_element = ""
        else:
            if char == close_char:
                depth -= 1
                # Stop after finding the first level
                if depth == level - 1:
                    if current_element.strip():
                        elements.append(current_element.strip())
                    return elements, (start, index)
            if depth >= level:
                current_element += char
            if char == open_char:
                depth += 1
                
                if depth == level:
                    start = index
        index += 1
    
    if current_element.strip():
        elements.append(current_element.strip())
    
    return elements, (start, index)

def is_pos_in_string(text: str, pos: int) -> bool:
    """Check if a position in text is inside a string literal.
    
    Args:
        text (str): The text to analyze
        pos (int): Position to check
        
    Returns:
        bool: True if position is inside a string literal, False otherwise
        
    Note:
        Handles escaped quotes by counting preceding backslashes
    """
    in_string = False
    i = 0
    while i < pos:
        char = text[i]
        if char == '"':
            # Check if quote is escaped by backslash
            backslash_count = 0
            j = i - 1
            while j >= 0 and text[j] == '\\':
                backslash_count += 1
                j -= 1
            if backslash_count % 2 == 0:  # even number means not escaped
                in_string = not in_string
        i += 1
    return in_string