"""Base construct class."""

from ...common.annotations import equals_hash


@equals_hash
class Construct:
    """Base class for all C++ constructs."""
    
    def __init__(self, file_path: str = ""):
        """Initialize with source file path."""
        self.file_path = file_path