"""Function and Parameter constructs."""

from typing import List, Optional
from ..core.base import Construct, Definition
from ..types.types import Type, Value
from ..statements.statements import Statement
from .misc import Comment
from ....common.annotations import equals_hash


@equals_hash
class Parameter(Construct):
    """Function/constructor parameters with type, name, and default values."""
    
    def __init__(self, name: str, param_type: Type, default_value: Optional[Value] = None):
        """Initialize parameter.
        
        Args:
            name (str): Parameter name
            param_type (Type): Parameter type
            default_value (Value, optional): Default value for parameter
        """
        super().__init__()
        self.name = name
        self.type = param_type
        self.default_value = default_value


@equals_hash
class Function(Definition):
    """Standalone functions and class methods with template parameters."""
    
    def __init__(self, name: str, return_type: Type, parameters: List[Parameter] = None,
                 body: List[Statement] = None, template_parameters: List[str] = None,
                 is_virtual: bool = False, is_static: bool = False, is_const: bool = False,
                 is_pure_virtual: bool = False, comment: Optional[Comment] = None, 
                 namespace: Optional[str] = None):
        """Initialize function.
        
        Args:
            name (str): Function name
            return_type (Type): Function return type
            parameters (List[Parameter], optional): Function parameters
            body (List[Statement], optional): Function body statements
            template_parameters (List[str], optional): Template parameter names
            is_virtual (bool): Whether function is virtual
            is_static (bool): Whether function is static
            is_const (bool): Whether function is const
            is_pure_virtual (bool): Whether function is pure virtual (= 0)
            comment (Comment, optional): Associated comment block
            namespace (str, optional): Namespace containing this function
        """
        super().__init__(name)
        self.return_type = return_type
        self.parameters = parameters or []
        self.body = body or []
        self.template_parameters = template_parameters or []
        self.is_virtual = is_virtual
        self.is_static = is_static
        self.is_const = is_const
        self.is_pure_virtual = is_pure_virtual  # Method ends with = 0
        self.comment = comment
        self.namespace = namespace
