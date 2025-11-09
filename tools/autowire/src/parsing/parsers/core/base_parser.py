"""Base parser class for all C++ parsers."""

from abc import ABC, abstractmethod
from typing import TypeVar, Generic
from ...constructs.core.base import Construct
from ..utils.context import ParseContext

T = TypeVar('T', bound=Construct)


class CPPParser(ABC, Generic[T]):
    """Abstract base class for all C++ parsers."""
    
    @abstractmethod
    def parse(self, context: ParseContext) -> T:
        """Parse C++ construct from current position in context.
        
        Args:
            context (ParseContext): Parsing context with file content, position, and utilities
            
        Returns:
            T: Parsed construct object or None if pattern doesn't match
            
        Raises:
            ParseError: If malformed syntax encountered during parsing
            
        Preconditions:
            - context.pos must point to valid position in context.content
            - Implementation must ONLY use ParseContext utility methods
            - Implementation must NEVER directly manipulate context fields (pos, content, symbol tables)
            
        Postconditions:
            - On success: context.pos advanced to end of parsed construct
            - On failure (None return): context.pos unchanged from entry
            - On ParseError: context.pos may be modified (dispatcher will handle backtracking)
            - Focus purely on parsing logic, position management handled by ParseContext methods
        """
        pass
    
    @abstractmethod
    def to_cpp(self, construct: T) -> str:
        """Serialize construct back to valid C++ code.
        
        Args:
            construct (T): Construct object to serialize
            
        Returns:
            str: Valid C++ code representation
            
        Preconditions:
            construct must be a valid instance of the expected type
        """
        pass
    
    def to_header(self, construct: T) -> str:
        """Serialize construct as header declaration.
        
        Args:
            construct (T): Construct object to serialize
            
        Returns:
            str: C++ header declaration code
            
        Preconditions:
            construct must be suitable for header declaration
        """
        return self.to_cpp(construct)
    
    def to_implementation(self, construct: T) -> str:
        """Serialize construct as implementation code.
        
        Args:
            construct (T): Construct object to serialize
            
        Returns:
            str: C++ implementation code
            
        Preconditions:
            construct must be suitable for implementation
        """
        return self.to_cpp(construct)


class ParseError(Exception):
    """Exception raised when parser encounters malformed syntax during parsing."""
    pass