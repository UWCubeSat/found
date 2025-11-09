"""Variable constructs."""

from typing import Optional
from ..core.base import Definition
from ..types.types import Type, Value
from ....common.annotations import equals_hash


@equals_hash
class Variable(Definition):
    """Member variables and global variables."""
    
    def __init__(self, name: str, var_type: Type, initial_value: Optional[Value] = None,
                 is_static: bool = False, is_const: bool = False, is_mutable: bool = False,
                 namespace: Optional[str] = None):
        """Initialize variable.
        
        Args:
            name (str): Variable name
            var_type (Type): Variable type
            initial_value (Value, optional): Initial value assignment
            is_static (bool): True if variable is static
            is_const (bool): True if variable is const
            is_mutable (bool): True if variable is mutable (can be modified in const contexts)
            namespace (str, optional): Namespace containing this variable
        """
        super().__init__(name)
        self.type = var_type
        self.initial_value = initial_value
        self.is_static = is_static
        self.is_const = is_const
        self.is_mutable = is_mutable
        self.namespace = namespace
