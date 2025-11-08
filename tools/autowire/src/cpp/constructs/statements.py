"""Statement hierarchy for code generation."""

from typing import List, Tuple, Optional
from .base import Construct
from ...common.annotations import equals_hash


@equals_hash
class Statement(Construct):
    """Base class for all statement constructs."""
    pass


@equals_hash
class SimpleStatement(Statement):
    """Single executable code line stored as raw C++ string (no parsing or semantic analysis)."""
    
    def __init__(self, code: str):
        super().__init__()
        self.code = code


@equals_hash
class IfStatement(Statement):
    """Conditional execution, inherits from Statement."""
    
    def __init__(self, condition: str, then_body: List[Statement],
                 else_if_clauses: List[Tuple[str, List[Statement]]] = None,
                 else_body: Optional[List[Statement]] = None):
        super().__init__()
        self.condition = condition
        self.then_body = then_body
        self.else_if_clauses = else_if_clauses or []
        self.else_body = else_body
    


@equals_hash
class ForLoop(Statement):
    """For loop construct, inherits from Statement."""
    
    def __init__(self, initialization: str, condition: str, increment: str, body: List[Statement]):
        super().__init__()
        self.initialization = initialization
        self.condition = condition
        self.increment = increment
        self.body = body


@equals_hash
class WhileLoop(Statement):
    """While loop construct, inherits from Statement."""
    
    def __init__(self, condition: str, body: List[Statement]):
        super().__init__()
        self.condition = condition
        self.body = body


@equals_hash
class SwitchStatement(Statement):
    """Switch statement, inherits from Statement."""
    
    def __init__(self, expression: str, cases: List[Tuple[str, List[Statement]]],
                 default_case: Optional[List[Statement]] = None):
        super().__init__()
        self.expression = expression
        self.cases = cases
        self.default_case = default_case