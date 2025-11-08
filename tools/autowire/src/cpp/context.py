"""ParseContext and symbol tables."""

from dataclasses import dataclass
from typing import Dict, Optional, Union
from .constructs import Comment, Namespace, Macro, Typedef, Using, Class, Function, Variable


@dataclass
class ParseContext:
    """Context maintained during parsing with symbol tables and state."""
    current_comment: Optional[Comment] = None
    current_namespace: Optional[Namespace] = None
    macro_table: Dict[str, Macro] = None
    alias_table: Dict[str, Union[Typedef, Using]] = None
    type_definitions: Dict[str, Class] = None
    function_definitions: Dict[str, Function] = None
    global_variables: Dict[str, Variable] = None
    static_variables: Dict[str, Variable] = None
    
    def __post_init__(self):
        """Initialize empty dictionaries."""
        if self.macro_table is None:
            self.macro_table = {}
        if self.alias_table is None:
            self.alias_table = {}
        if self.type_definitions is None:
            self.type_definitions = {}
        if self.function_definitions is None:
            self.function_definitions = {}
        if self.global_variables is None:
            self.global_variables = {}
        if self.static_variables is None:
            self.static_variables = {}