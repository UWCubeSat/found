"""Miscellaneous constructs (Namespace, Typedef, Using, Include, Macro, Comment)."""

from .base import Construct
from .types import Type
from ...common.annotations import equals_hash


@equals_hash
class Namespace(Construct):
    """Namespace declarations and usage."""
    
    def __init__(self, name: str):
        super().__init__()
        self.name = name


@equals_hash
class Typedef(Construct):
    """Type aliases and definitions."""
    
    def __init__(self, alias_name: str, original_type: Type):
        super().__init__()
        self.alias_name = alias_name
        self.original_type = original_type


@equals_hash
class Using(Construct):
    """Using declarations and using directives."""
    
    def __init__(self, target: str, is_directive: bool = False):
        super().__init__()
        self.target = target
        self.is_directive = is_directive


@equals_hash
class Include(Construct):
    """Preprocessor include directives."""
    
    def __init__(self, path: str, is_system: bool = False):
        super().__init__()
        self.path = path
        self.is_system = is_system


@equals_hash
class Macro(Construct):
    """Basic macro definitions (no conditional compilation)."""
    
    def __init__(self, name: str, definition: str = "", parameters: list = None):
        super().__init__()
        self.name = name
        self.definition = definition
        self.parameters = parameters or []


@equals_hash
class Comment(Construct):
    """Comment blocks with annotation extraction and entity association."""
    
    def __init__(self, text: str, is_multiline: bool = False, has_autowire: bool = False,
                 has_provider: bool = False):
        super().__init__()
        self.text = text
        self.is_multiline = is_multiline
        self.has_autowire = has_autowire
        self.has_provider = has_provider