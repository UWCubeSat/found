"""ParseContext and symbol tables."""

from dataclasses import dataclass
from typing import Dict, Optional, Union, TYPE_CHECKING
from .class_registry import ClassRegistry

if TYPE_CHECKING:
    from ..constructs import Comment, Namespace, Macro, Typedef, Using, Class, Function, Variable


@dataclass
class ParseContext:
    """Context maintained during parsing with symbol tables and state.
    
    Attributes:
        current_comment (Comment, optional): Currently active comment for association
        current_namespace (Namespace, optional): Current namespace scope
        macro_table (Dict[str, Macro]): Mapping of macro names to Macro constructs
        alias_table (Dict[str, Union[Typedef, Using]]): Type aliases and using declarations
        type_definitions (Dict[str, Class]): Class definitions by name
        function_definitions (Dict[str, Function]): Function definitions by name
        global_variables (Dict[str, Variable]): Global variable definitions
        static_variables (Dict[str, Variable]): Static variable definitions
        class_registry (ClassRegistry): Registry for inheritance analysis
    """
    current_comment: Optional['Comment'] = None
    current_namespace: Optional['Namespace'] = None
    macro_table: Dict[str, 'Macro'] = None
    alias_table: Dict[str, Union['Typedef', 'Using']] = None
    type_definitions: Dict[str, 'Class'] = None
    function_definitions: Dict[str, 'Function'] = None
    global_variables: Dict[str, 'Variable'] = None
    static_variables: Dict[str, 'Variable'] = None
    class_registry: ClassRegistry = None
    
    def __post_init__(self):
        """Initialize empty dictionaries and registries.
        
        Preconditions:
            None - all fields are optional and will be initialized if None
        """
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
        if self.class_registry is None:
            self.class_registry = ClassRegistry()