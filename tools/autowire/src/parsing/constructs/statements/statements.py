"""Statement hierarchy for code generation."""

from typing import List, Tuple, Optional
from ..core.base import Construct
from ....common.annotations import equals_hash


@equals_hash
class Statement(Construct):
    """Base class for all statement constructs."""
    
    def __init__(self, parent: Construct):
        """Initialize statement.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
        """
        super().__init__(parent)


@equals_hash
class SimpleStatement(Statement):
    """Single executable code line stored as raw C++ string (no parsing or semantic analysis)."""
    
    def __init__(self, parent: Construct, code: str):
        """Initialize simple statement.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
            code (str): Raw C++ code string
        """
        super().__init__(parent)
        self.code = code


@equals_hash
class IfStatement(Statement):
    """Conditional execution, inherits from Statement."""
    
    def __init__(self, parent: Construct, condition: str, then_body: List[Statement],
                 else_if_clauses: List[Tuple[str, List[Statement]]] = None,
                 else_body: Optional[List[Statement]] = None):
        """Initialize if statement.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
            condition (str): If condition expression
            then_body (List[Statement]): Statements to execute if condition is true
            else_if_clauses (List[Tuple[str, List[Statement]]], optional): Else-if clauses as (condition, statements) pairs
            else_body (List[Statement], optional): Else clause statements
        """
        super().__init__(parent)
        self.condition = condition
        self.then_body = then_body
        self.else_if_clauses = else_if_clauses or []
        self.else_body = else_body
    


@equals_hash
class ForLoop(Statement):
    """For loop construct, inherits from Statement."""
    
    def __init__(self, parent: Construct, initialization: str, condition: str, increment: str, body: List[Statement]):
        """Initialize for loop.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
            initialization (str): Loop initialization expression
            condition (str): Loop condition expression
            increment (str): Loop increment expression
            body (List[Statement]): Loop body statements
        """
        super().__init__(parent)
        self.initialization = initialization
        self.condition = condition
        self.increment = increment
        self.body = body


@equals_hash
class WhileLoop(Statement):
    """While loop construct, inherits from Statement."""
    
    def __init__(self, parent: Construct, condition: str, body: List[Statement]):
        """Initialize while loop.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
            condition (str): Loop condition expression
            body (List[Statement]): Loop body statements
        """
        super().__init__(parent)
        self.condition = condition
        self.body = body


@equals_hash
class SwitchStatement(Statement):
    """Switch statement, inherits from Statement."""
    
    def __init__(self, parent: Construct, expression: str, cases: List[Tuple[str, List[Statement]]],
                 default_case: Optional[List[Statement]] = None):
        """Initialize switch statement.
        
        Args:
            parent (Construct): Parent construct (typically Function or another Statement)
            expression (str): Switch expression
            cases (List[Tuple[str, List[Statement]]]): Case clauses as (value, statements) pairs
            default_case (List[Statement], optional): Default case statements
        """
        super().__init__(parent)
        self.expression = expression
        self.cases = cases
        self.default_case = default_case