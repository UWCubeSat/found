"""Miscellaneous constructs (Namespace, Typedef, Using, Include, Macro, Comment)."""

from typing import List
from ..core.base import Construct
from ..types.types import Type
from ....common.annotations import equals_hash


@equals_hash
class Namespace(Construct):
    """Namespace declarations and usage."""
    
    def __init__(self, parent: Construct, name: str):
        """Initialize namespace.
        
        Args:
            parent (Construct): Parent construct (typically File)
            name (str): Namespace name
        """
        super().__init__(parent)
        self.name = name


@equals_hash
class Typedef(Construct):
    """Type aliases and definitions."""
    
    def __init__(self, parent: Construct, alias_name: str, original_type: Type):
        """Initialize typedef.
        
        Args:
            parent (Construct): Parent construct (typically File or Namespace)
            alias_name (str): New type alias name
            original_type (Type): Original type being aliased
        """
        super().__init__(parent)
        self.alias_name = alias_name
        self.original_type = original_type


@equals_hash
class Using(Construct):
    """Using declarations and using directives."""
    
    def __init__(self, parent: Construct, target: str, is_directive: bool = False):
        """Initialize using declaration/directive.
        
        Args:
            parent (Construct): Parent construct (typically File or Namespace)
            target (str): Target namespace or type being used
            is_directive (bool): True for 'using namespace', False for 'using Type'
        """
        super().__init__(parent)
        self.target = target
        self.is_directive = is_directive


@equals_hash
class Include(Construct):
    """Preprocessor include directives."""
    
    def __init__(self, parent: Construct, path: 'File', is_system: bool = False):
        """Initialize include directive.
        
        Args:
            parent (Construct): Parent construct (typically File)
            path (str): Include path
            is_system (bool): True for <> includes, False for "" includes
        """
        super().__init__(parent)
        self.path = path
        self.is_system = is_system


@equals_hash
class Macro(Construct):
    """Basic macro definitions (no conditional compilation)."""
    
    def __init__(self, parent: Construct, name: str, definition: str = "", parameters: List[str] = None):
        """Initialize macro definition.
        
        Args:
            parent (Construct): Parent construct (typically File)
            name (str): Macro name
            definition (str): Macro definition/replacement text
            parameters (List[str], optional): Macro parameter names for function-like macros
        """
        super().__init__(parent)
        self.name = name
        self.definition = definition
        self.parameters = parameters or []


@equals_hash
class Comment(Construct):
    """Comment blocks with annotation extraction and entity association."""
    
    def __init__(self, parent: Construct, text: str, is_multiline: bool = False):
        """Initialize comment block.
        
        Args:
            parent (Construct): Parent construct (any construct type)
            text (str): Comment text content
            is_multiline (bool): True for /* */ comments, False for // comments
        """
        super().__init__(parent)
        self.text = text
        self.is_multiline = is_multiline