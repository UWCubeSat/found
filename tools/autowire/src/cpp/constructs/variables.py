"""Variable and Enum constructs."""

from typing import List, Optional
from .base import Construct
from .types import Type, Value
from .misc import Comment
from ...common.annotations import equals_hash


@equals_hash
class Variable(Construct):
    """Member variables and global variables."""
    
    def __init__(self, name: str, var_type: Type, initial_value: Optional[Value] = None,
                 is_static: bool = False, is_const: bool = False, is_mutable: bool = False,
                 comment: Optional[Comment] = None, namespace: Optional[str] = None):
        super().__init__()
        self.name = name
        self.type = var_type
        self.initial_value = initial_value
        self.is_static = is_static
        self.is_const = is_const
        self.is_mutable = is_mutable
        self.comment = comment
        self.namespace = namespace


@equals_hash
class Enum(Construct):
    """Enum and enum class definitions with values."""
    
    def __init__(self, name: str, values: List[str], is_class: bool = False,
                 underlying_type: Optional[Type] = None, comment: Optional[Comment] = None,
                 namespace: Optional[str] = None):
        super().__init__()
        self.name = name
        self.values = values
        self.is_class = is_class
        self.underlying_type = underlying_type
        self.comment = comment
        self.namespace = namespace