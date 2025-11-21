"""Custom annotations for autowire system."""

from .equals_hash import equals_hash
from .repr import to_string
from .auto_properties import auto_init

__all__ = ['equals_hash', 'auto_init']