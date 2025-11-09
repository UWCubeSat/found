"""File construct for representing parsed source files."""

from typing import List, Tuple
from .base import Construct, Definition
from ..definitions.classes import Class, Constructor, Destructor
from ..definitions.enums import Enum
from ..definitions.functions import Function
from ..statements.variables import Variable
from ..definitions.misc import Namespace, Typedef, Using, Include, Macro, Comment
from ....common.annotations import equals_hash


@equals_hash
class File(Definition):
    """Represents a parsed source file with all its constructs."""
    
    def __init__(self, file_path: str):
        """Initialize file construct.
        
        Args:
            file_path (str): Absolute path to the source file
            
        Preconditions:
            file_path must be an absolute path
        """
        # File is the only construct that can have no parent
        super().__init__(file_path)
        self.comments = []  # Comments associated with the file itself
        
        # All constructs in file order with position
        self.constructs: List[Tuple[Construct, int]] = []
        
        # Pre-filtered construct collections for easy access
        self.classes: List[Tuple[Class, int]] = []
        self.functions: List[Tuple[Function, int]] = []
        self.constructors: List[Tuple[Constructor, int]] = []
        self.destructors: List[Tuple[Destructor, int]] = []
        self.variables: List[Tuple[Variable, int]] = []
        self.enums: List[Tuple[Enum, int]] = []
        self.namespaces: List[Tuple[Namespace, int]] = []
        self.typedefs: List[Tuple[Typedef, int]] = []
        self.using_declarations: List[Tuple[Using, int]] = []
        self.includes: List[Tuple[Include, int]] = []
        self.macros: List[Tuple[Macro, int]] = []
        self.all_comments: List[Tuple[Comment, int]] = []  # All comments with position numbers
        
        # Comment location mapping for associating comments with constructs
        self.comment_associations = {}  # Maps position numbers to constructs
    
    def add_construct(self, construct: Construct, position: int) -> None:
        """Add a construct to the file and appropriate filtered list.
        
        Args:
            construct (Construct): Construct to add to the file
            position (int): Position where construct appears in source
            
        Preconditions:
            position must be positive
        """
        # Set this file as parent for top-level constructs
        construct.parent = self
        
        # Add to main list
        self.constructs.append((construct, position))
        
        # Add to appropriate filtered list
        if isinstance(construct, Class):
            self.classes.append((construct, position))
        elif isinstance(construct, Function):
            self.functions.append((construct, position))
        elif isinstance(construct, Constructor):
            self.constructors.append((construct, position))
        elif isinstance(construct, Destructor):
            self.destructors.append((construct, position))
        elif isinstance(construct, Variable):
            self.variables.append((construct, position))
        elif isinstance(construct, Enum):
            self.enums.append((construct, position))
        elif isinstance(construct, Namespace):
            self.namespaces.append((construct, position))
        elif isinstance(construct, Typedef):
            self.typedefs.append((construct, position))
        elif isinstance(construct, Using):
            self.using_declarations.append((construct, position))
        elif isinstance(construct, Include):
            self.includes.append((construct, position))
        elif isinstance(construct, Macro):
            self.macros.append((construct, position))
        elif isinstance(construct, Comment):
            self.all_comments.append((construct, position))
    
    def get_constructs_by_type(self, construct_type) -> List[Tuple[Construct, int]]:
        """Get all constructs of a specific type.
        
        Args:
            construct_type: Type class to filter by
            
        Returns:
            List[Tuple[Construct, int]]: List of (construct, position) pairs matching the type
        """
        return [(c, pos) for c, pos in self.constructs if isinstance(c, construct_type)]
    
    def associate_comment_with_construct(self, comment: Comment, construct: Construct) -> None:
        """Associate a comment with a specific construct.
        
        Args:
            comment (Comment): Comment to associate
            construct (Construct): Target construct to associate comment with
        """
        construct.add_comment(comment)
    
    def get_comments_on_line(self, position: int) -> List[Comment]:
        """Get all comments that appear on a specific line.
        
        Args:
            position (int): Position to search for comments
            
        Returns:
            List[Comment]: Comments found on the specified position
        """
        return [comment for comment, pos in self.all_comments if pos == position]
    
    def associate_comments_by_proximity(self) -> None:
        """Associate comments with constructs based on line proximity.
        
        Comments are associated with the nearest construct that follows them,
        or with constructs on the same position.
        """
        for comment, comment_pos in self.all_comments:
            # Find constructs on the same line or immediately following
            for construct, construct_pos in self.constructs:
                if construct_pos == comment_pos or construct_pos == comment_pos + 1:
                    self.associate_comment_with_construct(comment, construct)
                    break
            else:
                # If no construct found nearby, associate with file
                self.add_comment(comment)
    
    def get_qualified_name(self) -> str:
        return ""