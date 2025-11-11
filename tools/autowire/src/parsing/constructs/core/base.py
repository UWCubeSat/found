"""Base construct class."""

from ....common.annotations import equals_hash
from typing import List


@equals_hash
class Construct:
    """Base class for all C++ constructs."""
    
    def __init__(self):
        """Initialize construct"""
        self.parent = None
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
        return getattr(current, 'name', '')
    
    def get_root_file(self) -> 'File':
        """Get the root File containing this construct.
        
        Returns:
            File: Root File construct
            
        Preconditions:
            Construct should be part of a valid hierarchy with File as root
        """
        current = self
        while current.parent is not None:
            current = current.parent
        return current
    
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
    
    def set_parent(self, parent: 'Definition'):
        self.parent = parent

class Definition(Construct):
    """Base class for named C++ language constructs (classes, functions, variables, etc.).
    
    Represents any C++ construct that has a name and can be referenced.
    Provides qualified name resolution through the parent hierarchy.
    """
    
    def __init__(self, name: str):
        """Initialize definition with a name.
        
        Args:
            name (str): The name of this definition
        """
        super().__init__()
        self.name = name
    
    def get_qualified_name(self) -> str:
        """Get the fully qualified name including namespace hierarchy.

        Returns:
            str: Qualified name like "namespace::class::member" or just "name" for top-level
            
        Note:
            Traverses up the parent hierarchy to build the complete qualified name.
        """
        if self.parent is None:
            return self.name
        else:
            return self.parent.get_qualified_name() + "::" + self.name