"""Fast annotation detection using grep/ripgrep and other strategies."""

from typing import List, Dict
import subprocess
import os
import shutil
import re
from ..common.constants.discovery_constants import AUTOWIRE_PATTERN, PROVIDER_PATTERN, CPP_EXTENSIONS, HPP_EXT, CPP_EXT

class AnnotationScanner:
    """Fast annotation detection using various strategies."""
    
    def __init__(self, filepaths: List[str]):
        """Initialize scanner with file paths to scan.
        
        Args:
            filepaths (List[str]): List of file or directory paths to scan
        """
        self.filepaths = filepaths
        self._has_grep = shutil.which('grep') is not None
    
    def scan(self) -> Dict[str, tuple[str, bool, bool]]:
        """Scan documents for annotations.

        Returns:
            Dict[str, tuple[str, bool, bool]]: Mapping of file paths to (content, has_autowire, has_provider)
            Content is None when using grep strategy for performance
        """
        if self._has_grep:
            return self._grep_scan()
        return self._slow_scan()
    
    def _grep_scan(self) -> Dict[str, tuple[str, bool, bool]]:
        """Helper: Scan documents with grep for performance.

        Returns:
            Dict[str, tuple[str, bool, bool]]: Files with annotations, content is None for performance
            
        Preconditions:
            grep command must be available in system PATH
        """
        result = {}
            
        autowire_files = self._grep_for_annotation(AUTOWIRE_PATTERN)
        provider_files = self._grep_for_annotation(PROVIDER_PATTERN)
        
        # Build result
        all_files = autowire_files | provider_files
        for filepath in all_files:
            result[filepath] = (None, filepath in autowire_files, filepath in provider_files)
        return result
    
    def _slow_scan(self) -> Dict[str, tuple[str, bool, bool]]:
        """Helper: Scan documents by reading file contents directly.

        Returns:
            Dict[str, tuple[str, bool, bool]]: All C++ files with full content and annotation flags
            
        Preconditions:
            All file paths must be readable
        """
        result = {}
        for path in self.filepaths:
            try:
                for root, _, files in os.walk(path):
                    for file in files:
                        if file.endswith(CPP_EXTENSIONS):
                            filepath = os.path.join(root, file)
                            result[filepath] = self._scan_file(filepath)
            except:
                result[path] = self._scan_file(path)
        return result
    
    def _grep_for_annotation(self, annotation: str) -> set[str]:
        """Helper: Run grep for a specific annotation and return absolute file paths.
        
        Args:
            annotation (str): Regex pattern for the annotation to search for
            
        Returns:
            set[str]: Set of absolute file paths containing the annotation
            
        Preconditions:
            annotation must be a valid regex pattern
            grep command must be available
        """
        cmd = ['grep', '-rlE', f'--include=*{HPP_EXT}', f'--include=*{CPP_EXT}', annotation] + self.filepaths
        result = subprocess.run(cmd, capture_output=True, text=True)
        return {os.path.abspath(f) for f in result.stdout.strip().split('\n') 
               if result.stdout.strip() and f.strip()}
    
    def _scan_file(self, filepath: str) -> tuple[str, bool, bool]:
        """Helper: Scan a single file for annotations.
        
        Args:
            filepath (str): Path to the file to scan
            
        Returns:
            tuple[str, bool, bool]: (file_content, has_autowire, has_provider)
            Returns (None, False, False) for non-C++ files or read errors
            
        Preconditions:
            filepath should point to a readable file
        """
        if not filepath.endswith(CPP_EXTENSIONS):
            return (None, False, False)
        try:
            with open(filepath, 'r') as f:
                content = f.read()
                return (content, bool(re.search(AUTOWIRE_PATTERN, content)), bool(re.search(PROVIDER_PATTERN, content)))
        except:
            return (None, False, False)
