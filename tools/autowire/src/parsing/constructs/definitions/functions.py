"""Function and Parameter constructs."""

from dataclasses import dataclass, field
from typing import List, Optional
from ..core.base import Construct, Definition
from ..types.types import Type, Value
from ..statements.statements import Statement
from .misc import Comment


@dataclass
class Parameter(Construct):
    """Function/constructor parameters with type, name, and default values."""
    name: str
    type: Type
    default_value: Optional[Value] = None


@dataclass
class Function(Definition):
    """Standalone functions with template parameters."""
    name: str
    return_type: Type
    parameters: List['Parameter'] = field(default_factory=list)
    body: List[Statement] = field(default_factory=list)
    template_parameters: List[str] = field(default_factory=list)
    is_static: bool = False
    comment: Optional[Comment] = None
    namespace: Optional[str] = None
