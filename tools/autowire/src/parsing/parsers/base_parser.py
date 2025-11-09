"""Base parser class for all C++ parsers."""

from abc import ABC, abstractmethod
from typing import TypeVar, Generic
from ..constructs.base import Construct
from ..utils.context import ParseContext

T = TypeVar('T', bound=Construct)


class CPPParser(ABC, Generic[T]):
    """Abstract base class for all C++ parsers."""
    
    @abstractmethod
    def parse(self, statement: str, context: ParseContext) -> T:
        """Parse C++ code string into construct object.
        
        Args:
            statement (str): C++ code string to parse
            context (ParseContext): Parsing context with type information
            
        Returns:
            T: Parsed construct object
            
        Raises:
            ParseError: If statement cannot be parsed
            
        Preconditions:
            statement must be valid C++ syntax for this parser type
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