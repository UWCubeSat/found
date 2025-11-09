"""Enum constructs."""

from typing import List, Optional
from ..core.base import Construct
from ..types.types import Type
from .misc import Comment
from ....common.annotations import equals_hash


@equals_hash
class Enum(Construct):
    """Enum and enum class definitions with values."""
    
    def __init__(self, parent: Construct, name: str, values: List[str], is_class: bool = False,
                 underlying_type: Optional[Type] = None, comment: Optional[Comment] = None,
                 namespace: Optional[str] = None):
        """Initialize enum.
        
        Args:
            parent (Construct): Parent construct (typically Class or File)
            name (str): Enum name
            values (List[str]): Enum value names
            is_class (bool): True for 'enum class', False for 'enum'
            underlying_type (Type, optional): Underlying type for typed enums
            comment (Comment, optional): Associated comment block
            namespace (str, optional): Namespace containing this enum
        """
        super().__init__(parent)
        self.name = name
        self.values = values
        self.is_class = is_class
        self.underlying_type = underlying_type
        self.comment = comment
        self.namespace = namespace