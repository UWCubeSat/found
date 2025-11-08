"""Function and Parameter constructs."""

from typing import List, Optional
from .base import Construct
from .types import Type, Value
from .statements import Statement
from .misc import Comment
from ...common.annotations import equals_hash


@equals_hash
class Parameter(Construct):
    """Function/constructor parameters with type, name, and default values."""
    
    def __init__(self, name: str, param_type: Type, default_value: Optional[Value] = None):
        super().__init__()
        self.name = name
        self.type = param_type
        self.default_value = default_value


@equals_hash
class Function(Construct):
    """Standalone functions and class methods with template parameters."""
    
    def __init__(self, name: str, return_type: Type, parameters: List[Parameter] = None,
                 body: List[Statement] = None, template_parameters: List[str] = None,
                 is_virtual: bool = False, is_static: bool = False, is_const: bool = False,
                 comment: Optional[Comment] = None, namespace: Optional[str] = None):
        super().__init__()
        self.name = name
        self.return_type = return_type
        self.parameters = parameters or []
        self.body = body or []
        self.template_parameters = template_parameters or []
        self.is_virtual = is_virtual
        self.is_static = is_static
        self.is_const = is_const
        self.comment = comment
        self.namespace = namespace