"""Type and Value constructs."""

from dataclasses import dataclass, field
from typing import List
from ..core.base import Construct, Definition


@dataclass
class Type(Construct):
    """Fully qualified type information with template args, qualifiers, and source construct reference.
    This is different than a definition because it is defined whenever a type is being used, not defined
    
    Usage:
        Raw pointers:
            int* -> base_type=int, raw_pointer_level=1
            double** -> base_type=double, raw_pointer_level=2
        Smart pointers:
            std::unique_ptr<int> -> base_type='std::unique_ptr', template_args=[Type(base_type='int')]
        Auto types:
            auto x = getValue(); -> Type.AUTO
    """
    base_type: Definition
    template_args: List['Type'] = field(default_factory=list)
    raw_pointer_level: int = 0
    is_reference: bool = False
    is_const: bool = False
    
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
                base_type=self.base_type,
                template_args=self.template_args,
                raw_pointer_level=0,
                is_reference=self.is_reference,
                is_const=self.is_const
            )
        return self

Type.AUTO = Type(Definition('auto'))

@dataclass
class Value(Construct):
    """Literal values, expressions, initializers (strings, numbers, function calls, initializer lists)."""
    expression: Construct