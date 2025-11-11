
from typing import List
from ..core.base import Construct, Definition
from ..types.types import Type
from ....common.annotations import equals_hash


@equals_hash
class Namespace(Definition):
    """Namespace declarations and usage."""
    
    def __init__(self, name: str):
        """Initialize namespace.
        
        Args:
            name (str): Namespace name
        """
        super().__init__(name)


@equals_hash
class Typedef(Definition):
    """Type aliases and definitions."""
    
    def __init__(self, alias_name: str, original_type: Definition):
        """Initialize typedef.
        
        Args:
            alias_name (str): New type alias name
            original_type (Definition): Original type being aliased
        """
        super().__init__(alias_name)
        self.original_type = original_type


@equals_hash
class Using(Construct):
    """Using declarations and using directives."""
    
    def __init__(self, target: str, is_directive: bool = False):
        """Initialize using declaration/directive.
        
        Args:
            target (str): Target namespace or type being used
            is_directive (bool): True for 'using namespace', False for 'using Type'
        """
        super().__init__()
        self.target = target
        self.is_directive = is_directive


@equals_hash
class Include(Construct):
    """Preprocessor include directives."""
    
    def __init__(self, path: 'File', is_system: bool = False):
        """Initialize include directive.
        
        Args:
            path (str): Include path
            is_system (bool): True for <> includes, False for "" includes
        """
        super().__init__()
        self.path = path
        self.is_system = is_system


@equals_hash
class Comment(Construct):
    """Comment blocks with annotation extraction and entity association."""
    
    def __init__(self, text: str, is_multiline: bool = False):
        """Initialize comment block.
        
        Args:
            text (str): Comment text content
            is_multiline (bool): True for /* */ comments, False for // comments
        """
        super().__init__()
        self.text = text
        self.is_multiline = is_multiline