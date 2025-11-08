"""Class-related constructs."""

from typing import List, Optional
from .base import Construct
from .functions import Function, Parameter
from .variables import Variable
from .misc import Comment
from ...common.annotations import equals_hash


@equals_hash
class Constructor(Construct):
    """Class constructors with parameter lists."""
    
    def __init__(self, parameters: List[Parameter] = None):
        super().__init__()
        self.parameters = parameters or []


@equals_hash
class Destructor(Construct):
    """Class destructors (parsed but not used for dependency injection)."""
    
    def __init__(self, is_virtual: bool = False):
        super().__init__()
        self.is_virtual = is_virtual


@equals_hash
class AccessSection(Construct):
    """Container for class members with specific access level."""
    
    def __init__(self):
        super().__init__()
        self.constructors: List[Constructor] = []
        self.methods: List[Function] = []
        self.members: List[Variable] = []


@equals_hash
class Class(Construct):
    """Class and struct definitions with inheritance, constructors, methods, access specifiers."""
    
    def __init__(self, name: str, is_struct: bool = False, base_classes: List[str] = None,
                 template_parameters: List[str] = None, can_brace_initialize: bool = False,
                 brace_init_members: List[Variable] = None, comment: Optional[Comment] = None,
                 namespace: Optional[str] = None):
        super().__init__()
        self.name = name
        self.is_struct = is_struct
        self.base_classes = base_classes or []
        self.template_parameters = template_parameters or []
        self.can_brace_initialize = can_brace_initialize
        self.brace_init_members = brace_init_members or []
        self.comment = comment
        self.namespace = namespace
        
        # Access level buckets
        self.public = AccessSection()
        self.private = AccessSection()
        self.protected = AccessSection()