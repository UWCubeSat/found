"""Variable constructs."""

from dataclasses import dataclass
from typing import Optional
from ..core.base import Definition
from ..types.types import Type, Value


@dataclass
class Variable(Definition):
    """Member variables and global variables."""
    name: str
    type: Type
    initial_value: Optional[Value] = None
    is_static: bool = False
    is_const: bool = False
    is_mutable: bool = False
    namespace: Optional[str] = None
