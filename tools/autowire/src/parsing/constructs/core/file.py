"""File construct for representing parsed source files."""

from typing import List, Tuple
from .base import Construct
from ..definitions.classes import Class, Constructor, Destructor
from ..definitions.enums import Enum
from ..definitions.functions import Function
from ..statements.variables import Variable
from ..definitions.misc import Namespace, Typedef, Using, Include, Macro, Comment
from ....common.annotations import equals_hash


@equals_hash
class File(Construct):
    """Represents a parsed source file with all its constructs."""
    
    def __init__(self, file_path: str):
        """Initialize file construct.
        
        Args:
            file_path (str): Absolute path to the source file
            
        Preconditions:
            file_path must be an absolute path
        """
        # File is the only construct that can have no parent
        self.parent = None
        self.file_path = file_path
        self.comments = []  # Comments associated with the file itself
        
        # All constructs in file order with line numbers
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
        self.all_comments: List[Tuple[Comment, int]] = []  # All comments with line numbers
        
        # Comment location mapping for associating comments with constructs
        self.comment_associations = {}  # Maps line numbers to constructs
    
    def add_construct(self, construct: Construct, line_number: int) -> None:
        """Add a construct to the file and appropriate filtered list.
        
        Args:
            construct (Construct): Construct to add to the file
            line_number (int): Line number where construct appears in source
            
        Preconditions:
            line_number must be positive
        """
        # Set this file as parent for top-level constructs
        construct.parent = self
        
        # Add to main list
        self.constructs.append((construct, line_number))
        
        # Add to appropriate filtered list
        if isinstance(construct, Class):
            self.classes.append((construct, line_number))
        elif isinstance(construct, Function):
            self.functions.append((construct, line_number))
        elif isinstance(construct, Constructor):
            self.constructors.append((construct, line_number))
        elif isinstance(construct, Destructor):
            self.destructors.append((construct, line_number))
        elif isinstance(construct, Variable):
            self.variables.append((construct, line_number))
        elif isinstance(construct, Enum):
            self.enums.append((construct, line_number))
        elif isinstance(construct, Namespace):
            self.namespaces.append((construct, line_number))
        elif isinstance(construct, Typedef):
            self.typedefs.append((construct, line_number))
        elif isinstance(construct, Using):
            self.using_declarations.append((construct, line_number))
        elif isinstance(construct, Include):
            self.includes.append((construct, line_number))
        elif isinstance(construct, Macro):
            self.macros.append((construct, line_number))
        elif isinstance(construct, Comment):
            self.all_comments.append((construct, line_number))
    
    def get_constructs_by_type(self, construct_type) -> List[Tuple[Construct, int]]:
        """Get all constructs of a specific type.
        
        Args:
            construct_type: Type class to filter by
            
        Returns:
            List[Tuple[Construct, int]]: List of (construct, line_number) pairs matching the type
        """
        return [(c, line) for c, line in self.constructs if isinstance(c, construct_type)]
    
    def associate_comment_with_construct(self, comment: Comment, construct: Construct) -> None:
        """Associate a comment with a specific construct.
        
        Args:
            comment (Comment): Comment to associate
            construct (Construct): Target construct to associate comment with
        """
        construct.add_comment(comment)
    
    def get_comments_on_line(self, line_number: int) -> List[Comment]:
        """Get all comments that appear on a specific line.
        
        Args:
            line_number (int): Line number to search for comments
            
        Returns:
            List[Comment]: Comments found on the specified line
        """
        return [comment for comment, line in self.all_comments if line == line_number]
    
    def associate_comments_by_proximity(self) -> None:
        """Associate comments with constructs based on line proximity.
        
        Comments are associated with the nearest construct that follows them,
        or with constructs on the same line.
        """
        for comment, comment_line in self.all_comments:
            # Find constructs on the same line or immediately following
            for construct, construct_line in self.constructs:
                if construct_line == comment_line or construct_line == comment_line + 1:
                    self.associate_comment_with_construct(comment, construct)
                    break
            else:
                # If no construct found nearby, associate with file
                self.add_comment(comment)