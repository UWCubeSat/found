"""Base parser class for all C++ parsers."""

from abc import ABC, abstractmethod
from typing import TypeVar, Generic
from ..constructs.base import Construct
from ..context import ParseContext

T = TypeVar('T', bound=Construct)


class CPPParser(ABC, Generic[T]):
    """Abstract base class for all C++ parsers."""
    
    @abstractmethod
    def parse(self, statement: str, context: ParseContext) -> T:
        """Parse C++ code string into construct object."""
        pass
    
    @abstractmethod
    def to_cpp(self, construct: T) -> str:
        """Serialize construct back to valid C++ code."""
        pass
    
    def to_header(self, construct: T) -> str:
        """Serialize construct as header declaration."""
        return self.to_cpp(construct)
    
    def to_implementation(self, construct: T) -> str:
        """Serialize construct as implementation code."""
        return self.to_cpp(construct)