"""Project file cache and file info structures."""

from dataclasses import dataclass
from typing import Dict, List
import os
from ..common.annotations import equals_hash


@dataclass(eq= True, frozen=True)
class FileInfo:
    """Information about a source file.
    
    Args:
        file_path: Absolute path to the source file (must be absolute)
        file_content: Complete content of the file as string
        has_autowire: True if file contains [[AUTOWIRE]] annotations
        has_provider: True if file contains [[PROVIDER]] annotations
    
    Raises:
        ValueError: If file_path is not an absolute path
    """
    file_path: str
    file_content: str
    has_autowire: bool
    has_provider: bool
    
    def __post_init__(self):
        if not os.path.isabs(self.file_path):
            raise ValueError(f"file_path must be absolute, got: {self.file_path}")
    
    @classmethod
    def get_file(cls, file_path: str, has_autowire: bool = False, has_provider: bool = False) -> 'FileInfo':
        with open(file_path, 'r') as f:
            return FileInfo(file_path, f.read(), has_autowire, has_provider)
                


@equals_hash
class ProjectFileCache:
    """Cache containing all project files with pre-filtering."""
    
    def __init__(self,
                 root_path: str,
                 all_files: Dict[str, FileInfo], 
                 autowire_files: List[str],
                 provider_files: List[str]):
        """Initialize cache with pre-filtered file collections.
        
        Args:
            all_files (Dict[str, FileInfo]): Dictionary mapping absolute file paths to FileInfo objects
            autowire_files (List[str]): List of absolute file paths that contain [[AUTOWIRE]] annotations
            provider_files (List[str]): List of absolute file paths that contain [[PROVIDER]] annotations
            
        Preconditions:
            All file paths must be absolute paths
            autowire_files and provider_files must be subsets of all_files keys
        """
        self._root_path = root_path
        self._all_files = all_files
        self._autowire_files = sorted(autowire_files)
        self._provider_files = sorted(provider_files)
    
    @property
    def autowire_files(self) -> List[str]:
        """List of file paths with has_autowire = True.
        
        Returns:
            List[str]: Sorted list of absolute file paths containing AUTOWIRE annotations
        """
        return self._autowire_files
    
    @property
    def provider_files(self) -> List[str]:
        """List of file paths with has_provider = True.
        
        Returns:
            List[str]: Sorted list of absolute file paths containing PROVIDER annotations
        """
        return self._provider_files
    
    def get_all_files(self) -> Dict[str, FileInfo]:
        """Internal storage for all project files.
        
        Returns:
            Dict[str, FileInfo]: Dictionary mapping file paths to FileInfo objects
        """
        return self._all_files
    
    def get_file_content(self, file_path: str) -> str:
        """Access any file's content.
        
        Args:
            file_path (str): Absolute path to the file
            
        Returns:
            str: Complete file content
            
        Raises:
            KeyError: If file_path not found in cache
            
        Preconditions:
            file_path must exist in the cache
        """
        return self._all_files[file_path].file_content
    
    def __contains__(self, file_path: str) -> bool:
        """Check if file exists in cache.
        
        Args:
            file_path (str): Absolute path to check
            
        Returns:
            bool: True if file exists in cache
        """
        return file_path in self._all_files
    
    def add_file(self, file_info: FileInfo) -> None:
        """Add a file to the cache.
        
        Args:
            file_info (FileInfo): FileInfo object containing file data and annotation flags
            
        Preconditions:
            file_info.file_path must be an absolute path
        """
        self._all_files[file_info.file_path] = file_info
        
        if file_info.has_autowire and file_info.file_path not in self._autowire_files:
            self._autowire_files.append(file_info.file_path)
        if file_info.has_provider and file_info.file_path not in self._provider_files:
            self._provider_files.append(file_info.file_path)
    
