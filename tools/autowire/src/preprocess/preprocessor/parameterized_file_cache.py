"""
Preprocessing cache for handling macro-aware file processing.
"""

from ..constructs.parameterized_file import ParameterizedFile
from ..constructs.macros import DefineDirective
from ...common.annotations import equals_hash, to_string
from typing import List, Optional

@equals_hash
@to_string
class ParameterizedFileCache:
    """Cache for preprocessed files with different macro contexts.
    
    Stores multiple versions of the same file preprocessed with different
    macro definitions. Uses macro hash for efficient lookup and prevents
    redundant preprocessing of identical macro contexts.
    """
    
    def __init__(self):
        """Initialize empty cache."""
        self.files = {}  # file_path -> Dict[hash, List[Tuple[List[DefineDirective], ParameterizedFile]]]
    
    def get_file(self, file_path: str, macros: List[DefineDirective] = None) -> Optional[ParameterizedFile]:
        """Retrieve cached preprocessed file for given macro context.
        
        Args:
            file_path (str): Path to the source file
            macros (List[DefineDirective], optional): Macro context for preprocessing
            
        Returns:
            Optional[ParameterizedFile]: Cached file if found, None otherwise
        """
        if file_path not in self.files:
            return None
            
        hash_code = hash(frozenset(macros or []))
        if hash_code not in self.files[file_path]:
            return None
        
        for extern_macros, file in self.files[file_path][hash_code]:
            if extern_macros == macros:
                return file

        return None
    
    def put_file(self, file: ParameterizedFile, macros: List[DefineDirective] = None) -> bool:
        """Store preprocessed file in cache with macro context.
        
        Args:
            file (ParameterizedFile): Preprocessed file to cache
            macros (List[DefineDirective], optional): Macro context used for preprocessing
            
        Returns:
            bool: True if file was added, False if already exists
        """
        if file.file_path not in self.files:
            self.files[file.file_path] = {}
            
        hash_code = hash(frozenset(macros or []))
        if hash_code not in self.files[file.file_path]:
            self.files[file.file_path][hash_code] = []
        
        entries = self.files[file.file_path][hash_code]
        for i in range(len(entries)):
            extern_macros, _ = entries[i]
            if extern_macros == macros:
                return False
        entries.append((macros, file))
        return True