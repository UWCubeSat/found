"""Type and Value constructs."""

from typing import List
from ..core.base import Construct
from ....common.annotations import equals_hash


@equals_hash
class Type(Construct):
    """Fully qualified type information with template args, qualifiers, and source construct reference.
    This is different than a definition because it is defined whenever a type is being used, not defined
    
    Usage:
        Raw pointers:
            int* -> qualified_name=int, raw_pointer_level=1
            double** -> qualified_name=double, raw_pointer_level=2
        Smart pointers:
            std::unique_ptr<int> -> qualified_name='std::unique_ptr', template_args=[Type(qualified_name='int')]
    """
    
    def __init__(self, parent: Construct, qualified_name: str, template_args: List['Type'] = None,
                 raw_pointer_level: int = 0, is_reference: bool = False, is_const: bool = False):
        """Initialize type.
        
        Args:
            parent (Construct): Parent construct (typically Parameter, Variable, or Function)
            qualified_name (str): Namespace-qualified type name ('MyClass', 'std::unique_ptr', 'ns::MyClass')
            template_args (List[Type], optional): Template arguments for generic types
            raw_pointer_level (int): Number of raw pointer levels (0 for T, 1 for T*, 2 for T**)
            is_reference (bool): True if this is a reference type
            is_const (bool): True if this type is const-qualified
        """
        super().__init__(parent)
        self.qualified_name = qualified_name
        self.template_args = template_args or []
        self.raw_pointer_level = raw_pointer_level
        self.is_reference = is_reference
        self.is_const = is_const
    
    @property
    def is_pointer(self) -> bool:
        """Check if this type is a raw pointer (not smart pointer).
        
        Returns:
            bool: True if this type has raw pointer indirection
        """
        return self.raw_pointer_level > 0
    
    def get_pointer_level(self) -> int:
        """Get the depth of raw pointer indirection (not smart pointer).
        
        Returns:
            int: Number of raw pointer levels (0 for non-pointers, 1 for T*, 2 for T**, etc.)
        """
        return self.raw_pointer_level
    
    def get_base_type(self) -> 'Type':
        """Get the base type without raw pointer indirection (not smart pointer).
        
        Returns:
            Type: The same type but with raw_pointer_level=0
        """
        if self.raw_pointer_level > 0:
            return Type(
                parent=self.parent,
                qualified_name=self.qualified_name,
                template_args=self.template_args,
                raw_pointer_level=0,
                is_reference=self.is_reference,
                is_const=self.is_const
            )
        return self

@equals_hash
class Value(Construct):
    """Literal values, expressions, initializers (strings, numbers, function calls, initializer lists)."""
    
    def __init__(self, parent: Construct, expression: str):
        """Initialize value.
        
        Args:
            parent (Construct): Parent construct (typically Parameter or Variable)
            expression (str): Value expression (literal, function call, etc.)
        """
        super().__init__(parent)
        self.expression = expression