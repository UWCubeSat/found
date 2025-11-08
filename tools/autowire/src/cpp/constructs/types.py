"""Type and Value constructs."""

from typing import List, Optional, Union
from .base import Construct
from ...common.annotations import equals_hash


@equals_hash
class Type(Construct):
    """Fully qualified type information with template args, qualifiers, and source construct reference."""
    
    def __init__(self, qualified_name: str, source_construct: Optional[Union['Class', 'Typedef', 'Using']] = None,
                 template_args: List['Type'] = None, is_pointer: bool = False, 
                 is_reference: bool = False, is_const: bool = False):
        super().__init__()
        self.qualified_name = qualified_name
        self.source_construct = source_construct
        self.template_args = template_args or []
        self.is_pointer = is_pointer
        self.is_reference = is_reference
        self.is_const = is_const
    


@equals_hash
class Value(Construct):
    """Literal values, expressions, initializers (strings, numbers, function calls, initializer lists)."""
    
    def __init__(self, expression: str):
        super().__init__()
        self.expression = expression