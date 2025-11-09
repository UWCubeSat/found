"""Code generator for dependency injection factories and CLI parsers."""

from typing import List
from ..parsing.constructs import Class, Function, Construct


class CodeGenerator:
    """Generates C++ dependency injection code using construct serialization."""
    
    def generate_factory_functions(self, autowire_classes: List[Class]) -> List[Function]:
        """Create factory functions for AUTOWIRE classes.
        
        Args:
            autowire_classes (List[Class]): Classes requiring dependency injection
            
        Returns:
            List[Function]: Generated factory functions as Function constructs
            
        Preconditions:
            All classes must have valid constructors with typed parameters
        """
        pass
    
    def generate_cli_parser(self, autowire_classes: List[Class]) -> Function:
        """Create command-line parser function.
        
        Args:
            autowire_classes (List[Class]): Classes to create CLI options for
            
        Returns:
            Function: Generated CLI parser function as Function construct
            
        Preconditions:
            autowire_classes must not be empty
        """
        pass
    
    def generate_provider_wiring(self, provider_functions: List[Function]) -> List[Function]:
        """Create dependency wiring functions.
        
        Args:
            provider_functions (List[Function]): Functions providing dependencies
            
        Returns:
            List[Function]: Generated wiring functions as Function constructs
            
        Preconditions:
            All provider functions must have valid return types
        """
        pass
    
    def write_generated_files(self, output_dir: str, constructs: List[Construct]) -> None:
        """Write .hpp/.cpp files using construct serialization.
        
        Args:
            output_dir (str): Directory to write generated files
            constructs (List[Construct]): Constructs to serialize to C++ code
            
        Raises:
            OSError: If output directory cannot be created or written to
            
        Preconditions:
            output_dir must be a valid directory path
            constructs must be serializable to C++ code
        """
        pass