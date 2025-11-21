"""Robust regex patterns for C++ parsing with proper whitespace handling.

C++ allows flexible whitespace in most contexts, so patterns must account for:
- Optional whitespace between tokens
- Newlines and multi-line constructs
- Comments between tokens
- Template parameter spacing
"""

import re

# Whitespace patterns
WS = r'\s*'  # Optional whitespace
WS_REQ = r'\s+'  # Required whitespace

# Basic C++ tokens with whitespace handling.
IDENTIFIER = r'[a-zA-Z_]\w*'
COMMON_MODIFIERS = r'(?:virtual|static|inline|const|mutable|explicit)'
QUALIFIED_NAME = rf'(?:::)?{IDENTIFIER}(?:{WS}<[^>]*>)?(?:{WS}::{WS}{IDENTIFIER}(?:{WS}<[^>]*>)?)*'  # must never be used at the end of patterns or by itself

# Type patterns - no outer parentheses, used as building block. Must always be followed by {WS_REQ}
# Handles: regular types, function pointers, member function pointers
TYPE_PATTERN = rf'(?:{QUALIFIED_NAME}|{QUALIFIED_NAME}{WS}\([^\)]*\)\([^\)]*\)|{QUALIFIED_NAME}{WS}\(\*[^\)]*\)\([^\)]*\)){WS}(?:[&*]+{WS})*'

# Class/struct/union patterns - captures: template_params, class_type, name, inheritance
CLASS_PATTERN = rf'{WS}(?:template{WS_REQ}<([^>]*)>{WS})?(class|struct|union){WS_REQ}({IDENTIFIER})(?:{WS}:{WS}(.+?))?{WS}([{{;])'
CLASS_INHERITANCE = rf'(?:(public|private|protected){WS_REQ})?({QUALIFIED_NAME})'

# Function patterns - captures: template_params, modifiers, return_type, name
FUNCTION_PATTERN = rf'{WS}(?:template{WS_REQ}<([^>]*)>{WS})?((?:(?:{COMMON_MODIFIERS}){WS_REQ})*)({TYPE_PATTERN}){WS_REQ}({IDENTIFIER}){WS}\('
CONSTRUCTOR_PATTERN = rf'{WS}((?:(?:{COMMON_MODIFIERS}){WS_REQ})*)({IDENTIFIER}){WS}\('
DESTRUCTOR_PATTERN = rf'{WS}((?:(?:{COMMON_MODIFIERS}){WS_REQ})*)~({IDENTIFIER}){WS}\('
METHOD_PATTERN = rf'{WS}(?:template{WS_REQ}<([^>]*)>{WS})?((?:(?:{COMMON_MODIFIERS}){WS_REQ})*)({TYPE_PATTERN}){WS_REQ}(?:({IDENTIFIER})|operator{WS}([^\(]+)){WS}\('

# Variable patterns - captures: modifiers, type, name
VARIABLE_PATTERN = rf'{WS}((?:(?:{COMMON_MODIFIERS}){WS_REQ})*)({TYPE_PATTERN}){WS_REQ}({IDENTIFIER})'

# Access specifiers - captures: access_level
ACCESS_PATTERN = rf'{WS}(public|private|protected){WS}:'

# Enum patterns - captures: enum_type, name, underlying_type
ENUM_PATTERN = rf'{WS}(enum(?:{WS_REQ}class|{WS_REQ}struct)?){WS_REQ}({IDENTIFIER})(?:{WS}:{WS}({QUALIFIED_NAME}))?{WS}([{{;])'

# Type alias patterns - just match keywords
TYPEDEF_PATTERN = rf'{WS}typedef{WS_REQ}'
USING_PATTERN = rf'{WS}using{WS_REQ}'

# Friend declaration pattern - just match keyword
FRIEND_PATTERN = rf'{WS}friend{WS_REQ}'

# Namespace patterns - captures: name
NAMESPACE_PATTERN = rf'{WS}namespace(?:{WS_REQ}({IDENTIFIER}))?{WS}\{{'

# Comment patterns - captures: comment_text
SINGLE_COMMENT = r'//([^\n]*)'
MULTI_COMMENT = r'/\*(.*?)\*/'
COMMENT_PATTERN = rf'(?://([^\n]*)|/\*(.*?)\*/)'

# Compiled patterns for performance
COMPILED_PATTERNS = {
    'class': re.compile(CLASS_PATTERN),
    'function': re.compile(FUNCTION_PATTERN),
    'constructor': re.compile(CONSTRUCTOR_PATTERN),
    'destructor': re.compile(DESTRUCTOR_PATTERN),
    'method': re.compile(METHOD_PATTERN),
    'variable': re.compile(VARIABLE_PATTERN),
    'access': re.compile(ACCESS_PATTERN),
    'enum': re.compile(ENUM_PATTERN),
    'typedef': re.compile(TYPEDEF_PATTERN),
    'using': re.compile(USING_PATTERN),
    'friend': re.compile(FRIEND_PATTERN),
    'namespace': re.compile(NAMESPACE_PATTERN),
    'comment': re.compile(COMMENT_PATTERN, re.DOTALL),
}

def match_pattern(pattern_name: str, content: str, pos: int = 0) -> re.Match:
    """Match compiled pattern at given position."""
    return COMPILED_PATTERNS[pattern_name].match(content, pos)

# Capture group indices for easy access
CAPTURE_GROUPS = {
    'class': {'template_params': 1, 'class_type': 2, 'name': 3, 'inheritance': 4, 'terminator': 5},
    'function': {'template_params': 1, 'modifiers': 2, 'return_type': 3, 'name': 4},
    'constructor': {'modifiers': 1, 'name': 2},
    'destructor': {'modifiers': 1, 'name': 2},
    'method': {'template_params': 1, 'modifiers': 2, 'return_type': 3, 'name': 4, 'operator': 5},
    'variable': {'modifiers': 1, 'type': 2, 'name': 3},

    'enum': {'enum_type': 1, 'name': 2, 'underlying_type': 3, 'terminator': 4},
    'typedef': {},
    'using': {},
    'friend': {},
    'namespace': {'name': 1},
    'access': {'level': 1},
    'comment': {'text': 1, 'multi_text': 2}
}