"""Union construct definitions for C++ parsing."""

from dataclasses import dataclass, field
from typing import List
from ..core.base import Construct


@dataclass
class Union(Construct):
    """Represents a C++ union declaration.
    
    A union is a collection of member variables that share the same memory location.
    Only one member can be active at a time.
    """
    name: str
    members: List = field(default_factory=list)
    
    def add_member(self, member) -> None:
        """Add a member variable to this union."""
        self.members.append(member)
    
