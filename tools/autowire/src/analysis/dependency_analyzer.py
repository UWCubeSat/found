"""Dependency analyzer for AUTOWIRE/PROVIDER matching."""

from typing import List
from ..cpp.parsers import ParsedFile
from ..cpp.constructs import Class, Function


class DependencyAnalyzer:
    """Analyzes dependency injection requirements and validates dependency graph."""
    
    def extract_autowire_classes(self, parsed_files: List[ParsedFile]) -> List[Class]:
        """Find classes with AUTOWIRE annotations."""
        pass
    
    def extract_provider_functions(self, parsed_files: List[ParsedFile]) -> List[Function]:
        """Find functions with PROVIDER annotations."""
        pass
    
    def get_constructor_dependencies(self, class_obj: Class) -> List[str]:
        """Extract parameter types from class constructors."""
        pass
    
    def validate_dependency_graph(self, autowire_classes: List[Class], provider_functions: List[Function]) -> List[str]:
        """Check for missing providers and circular dependencies."""
        pass