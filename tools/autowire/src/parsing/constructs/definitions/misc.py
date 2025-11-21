from dataclasses import dataclass
from typing import List
from ..core.base import Construct, Definition
from ..types.types import Type


@dataclass
class Namespace(Definition):
    """Namespace declarations and usage."""
    name: str


@dataclass
class Typedef(Definition):
    """Type aliases and definitions."""
    name: str
    original_type: Definition


@dataclass
class Using(Construct):
    """Using declarations and using directives."""
    target: str
    is_directive: bool = False


@dataclass
class Include(Construct):
    """Preprocessor include directives."""
    path: 'File'
    is_system: bool = False


@dataclass
class Comment(Construct):
    """Comment blocks with annotation extraction and entity association."""
    text: str
    is_multiline: bool = False