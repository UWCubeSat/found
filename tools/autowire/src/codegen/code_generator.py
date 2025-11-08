"""Code generator for dependency injection factories and CLI parsers."""

from typing import List
from ..cpp.constructs import Class, Function, Construct


class CodeGenerator:
    """Generates C++ dependency injection code using construct serialization."""
    
    def generate_factory_functions(self, autowire_classes: List[Class]) -> List[Function]:
        """Create factory functions for AUTOWIRE classes."""
        pass
    
    def generate_cli_parser(self, autowire_classes: List[Class]) -> Function:
        """Create command-line parser function."""
        pass
    
    def generate_provider_wiring(self, provider_functions: List[Function]) -> List[Function]:
        """Create dependency wiring functions."""
        pass
    
    def write_generated_files(self, output_dir: str, constructs: List[Construct]) -> None:
        """Write .hpp/.cpp files using construct serialization."""
        pass