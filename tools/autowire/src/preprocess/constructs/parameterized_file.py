from .macros import DefineDirective

from ...common.annotations import equals_hash, to_string

from typing import Dict, List
from copy import deepcopy

@to_string
@equals_hash
class ParameterizedFile:
    """A preprocessed C++ file with macro expansions and include resolution.
    
    Represents the result of preprocessing a source file with a specific macro
    context. Contains the expanded content, macro definitions, and dependency
    information for the translation unit.
    """

    def __init__(self, file_path: str, raw_content: str, defined_macros: Dict[str, DefineDirective], included_files: List['ParameterizedFile']):
        """Initialize a parameterized file with preprocessing results.
        
        Args:
            file_path (str): Absolute path to the source file
            raw_content (str): Preprocessed content with macros expanded
            defined_macros (Dict[str, DefineDirective]): Macros defined in this file
            included_files (List[ParameterizedFile]): Files included by this file
        """
        self.file_path = file_path
        self.raw_content = raw_content
        self.defined_macros: Dict[str, DefineDirective] = defined_macros
        self.included_files: List[ParameterizedFile] = included_files
    
    def reduce_macro(self) -> 'ParameterizedFile':
        """Create a copy with header guard macros removed from child files.
        
        Returns:
            ParameterizedFile: New instance with header guards filtered out
        """
        # Filter out header guard macros (ending with _HPP)
        filtered_macros = {
            name: macro for name, macro in self.defined_macros.items()
            if not name.endswith('_HPP')
        }
        
        # Recursively reduce macros in included files
        reduced_includes = [include.reduce_macro() for include in self.included_files]
        
        return ParameterizedFile(
            file_path=self.file_path,
            raw_content=self.raw_content,
            defined_macros=filtered_macros,
            included_files=reduced_includes
        )