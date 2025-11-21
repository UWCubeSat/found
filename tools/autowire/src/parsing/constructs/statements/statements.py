"""Statement hierarchy for code generation."""

from dataclasses import dataclass, field
from typing import List, Tuple, Optional
from ..core.base import Construct, Definition


@dataclass
class Statement(Construct):
    """Base class for all statement constructs."""
    pass


@dataclass
class SimpleStatement(Statement):
    """Single executable code line stored as raw C++ string (no parsing or semantic analysis)."""
    code: str


@dataclass
class IfStatement(Statement):
    """Conditional execution, inherits from Statement."""
    condition: str
    then_body: List['Statement']
    else_if_clauses: List[Tuple[str, List['Statement']]] = field(default_factory=list)
    else_body: Optional[List['Statement']] = None


@dataclass
class ForLoop(Statement):
    """For loop construct, inherits from Statement."""
    initialization: str
    condition: str
    increment: str
    body: List['Statement']


@dataclass
class WhileLoop(Statement):
    """While loop construct, inherits from Statement."""
    condition: str
    body: List['Statement']


@dataclass
class SwitchStatement(Statement):
    """Switch statement, inherits from Statement."""
    expression: str
    cases: List[Tuple[str, List['Statement']]]
    default_case: Optional[List['Statement']] = None