"""Base construct class."""

from ....common.annotations import equals_hash
from typing import Optional, List, TYPE_CHECKING

if TYPE_CHECKING:
    from ..definitions.misc import Comment


@equals_hash
class Construct:
    """Base class for all C++ constructs."""
    
    def __init__(self, parent: 'Construct'):
        """Initialize construct with required parent.
        
        Args:
            parent (Construct): Parent construct in the hierarchy
            
        Preconditions:
            parent must be a valid Construct instance (except for File which overrides this)
        """
        self.parent = parent
        self.comments = []  # List of Comment objects associated with this construct
    
    def get_file_path(self) -> str:
        """Get file path by traversing up to root File.
        
        Returns:
            str: Absolute path to the source file containing this construct
            
        Preconditions:
            Construct must be part of a valid hierarchy with File as root
        """
        current = self
        while current.parent is not None:
            current = current.parent
        # Root should be File which has file_path
        return getattr(current, 'file_path', '')
    
    def get_root_file(self) -> Optional['File']:
        """Get the root File containing this construct.
        
        Returns:
            File: Root File construct, or None if not in a valid hierarchy
            
        Preconditions:
            Construct should be part of a valid hierarchy with File as root
        """
        current = self
        while current.parent is not None:
            current = current.parent
        return current if hasattr(current, 'file_path') else None
    
    def add_comment(self, comment: 'Comment') -> None:
        """Add a comment to this construct.
        
        Args:
            comment (Comment): Comment to associate with this construct
        """
        self.comments.append(comment)
    
    def add_comments(self, comments: List['Comment']) -> None:
        """Add multiple comments to this construct.
        
        Args:
            comments (List[Comment]): Comments to associate with this construct
        """
        self.comments.extend(comments)