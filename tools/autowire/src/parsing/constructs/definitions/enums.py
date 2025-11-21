"""Enum constructs."""

from dataclasses import dataclass, field
from typing import List, Optional
from ..core.base import Definition
from ..types.types import Type, Value
from .misc import Comment


@dataclass
class EnumValue:
    """Individual enum value with optional explicit assignment."""
    name: str
    value: Optional[Value] = None  # None means auto-assigned


@dataclass
class Enum(Definition):
    """Enum and enum class definitions with values."""
    name: str
    values: List[EnumValue] = field(default_factory=list)
    is_class: bool = False
    underlying_type: Optional[Type] = None
    comment: Optional[Comment] = None
    namespace: Optional[str] = None