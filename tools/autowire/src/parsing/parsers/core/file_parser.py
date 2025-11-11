"""File-level parser for complete C++ source files."""

from .base_parser import CPPParser

class FileParser(CPPParser):
    """Parser for complete C++ source files using recursive descent parsing.
    
    Parses entire source files into File constructs containing all top-level
    constructs (classes, functions, variables, etc.) with proper parent-child
    relationships and symbol table management.
    """
    pass