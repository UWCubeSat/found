"""Robust regex patterns for C++ parsing with proper whitespace handling.

C++ allows flexible whitespace in most contexts, so patterns must account for:
- Optional whitespace between tokens
- Newlines and multi-line constructs
- Comments between tokens
- Template parameter spacing
"""

import re

# Whitespace pattern
WS = r'(?:\s|//[^\n]*\n|)*'  # Whitespace

# Basic C++ tokens with whitespace handling
IDENTIFIER = r'[a-zA-Z_]\w*'
QUALIFIED_NAME = rf'{IDENTIFIER}(?:{WS}::{WS}{IDENTIFIER})*'  # std::string, my::namespace::Class

# Class/struct patterns - captures: template_params, class_type, name, inheritance
CLASS_PATTERN = rf'{WS}(?:template{WS}<([^>]*)>{WS})?(class|struct){WS}({IDENTIFIER})(?:{WS}:{WS}(.+?))?{WS}\{{'
CLASS_INHERITANCE = rf'(?:(public|private|protected){WS})?({QUALIFIED_NAME})'

# Function patterns - captures: template_params, modifiers, return_type, name, parameters
FUNCTION_PATTERN = rf'{WS}(?:template{WS}<([^>]*)>{WS})?(?:(virtual|static|inline){WS})*({QUALIFIED_NAME}(?:{WS}[&*]+)*){WS}({IDENTIFIER}){WS}\(([^)]*)\)'
CONSTRUCTOR_PATTERN = rf'{WS}(?:(explicit){WS})?({IDENTIFIER}){WS}\(([^)]*)\)'

# Variable patterns - captures: modifiers, type, name, initializer
VARIABLE_PATTERN = rf'{WS}(?:(static|const|mutable){WS})*({QUALIFIED_NAME}(?:{WS}[&*]+)*){WS}({IDENTIFIER})(?:{WS}[=\{{]([^;\}}]+))?'

# Type patterns - captures: base_type, template_args, qualifiers
TYPE_PATTERN = rf'({QUALIFIED_NAME})(?:{WS}<([^>]*)>)?({WS}[&*]+)*'

# Access specifiers - captures: access_level
ACCESS_PATTERN = rf'{WS}(public|private|protected){WS}:'

# Namespace patterns - captures: name
NAMESPACE_PATTERN = rf'{WS}namespace(?:{WS}({IDENTIFIER}))?{WS}\{{'

# Preprocessor patterns - captures: filename, macro_name, macro_value, condition
INCLUDE_PATTERN = rf'{WS}#include{WS}[<"]([^>"]+)[>"]'
DEFINE_PATTERN = rf'{WS}#define{WS}({IDENTIFIER})(?:{WS}(.*))?'
IFDEF_PATTERN = rf'{WS}#(ifdef|ifndef|if){WS}(.+)'

# Comment patterns - captures: comment_text
SINGLE_COMMENT = r'//([^\n]*)'
MULTI_COMMENT = r'/\*(.*?)\*/'
COMMENT_PATTERN = rf'(?://([^\n]*)|/\*(.*?)\*/)'

# Compiled patterns for performance
COMPILED_PATTERNS = {
    'class': re.compile(CLASS_PATTERN),
    'function': re.compile(FUNCTION_PATTERN),
    'constructor': re.compile(CONSTRUCTOR_PATTERN),
    'variable': re.compile(VARIABLE_PATTERN),
    'access': re.compile(ACCESS_PATTERN),
    'namespace': re.compile(NAMESPACE_PATTERN),
    'include': re.compile(INCLUDE_PATTERN),
    'define': re.compile(DEFINE_PATTERN),
    'ifdef': re.compile(IFDEF_PATTERN),
    'comment': re.compile(COMMENT_PATTERN, re.DOTALL),
}

def match_pattern(pattern_name: str, content: str, pos: int = 0) -> re.Match:
    """Match compiled pattern at given position."""
    return COMPILED_PATTERNS[pattern_name].match(content, pos)

# Capture group indices for easy access
CAPTURE_GROUPS = {
    'class': {'template_params': 1, 'class_type': 2, 'name': 3, 'inheritance': 4},
    'function': {'template_params': 1, 'modifiers': 2, 'return_type': 3, 'name': 4, 'parameters': 5},
    'constructor': {'explicit': 1, 'name': 2, 'parameters': 3},
    'variable': {'modifiers': 1, 'type': 2, 'name': 3, 'initializer': 4},
    'type': {'base_type': 1, 'template_args': 2, 'qualifiers': 3},
    'include': {'filename': 1},
    'define': {'name': 1, 'value': 2},
    'ifdef': {'directive': 1, 'condition': 2},
    'namespace': {'name': 1},
    'access': {'level': 1},
    'comment': {'text': 1, 'multi_text': 2}
}