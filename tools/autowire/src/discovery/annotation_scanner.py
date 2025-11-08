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
        self.filepaths = filepaths
        self._has_grep = shutil.which('grep') is not None
    
    def scan(self) -> Dict[str, tuple[str, bool, bool]]:
        """Scan documents for annotations

        Returns:
            Dict[str, tuple[str, bool, bool]]: All files, with their content
            and notes if they have AUTOWIRE or PROVIDER annotations
        """
        if self._has_grep:
            return self._grep_scan()
        return self._slow_scan()
    
    def _grep_scan(self) -> Dict[str, tuple[str, bool, bool]]:
        """Helper: Scan documents with grep

        Returns:
            Dict[str, tuple[str, bool, bool]]: All files with content (None for grep),
            has AUTOWIRE, has PROVIDER annotations
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
        """Helper: Scan documents by opening all of them

        Returns:
            Dict[str, tuple[str, bool, bool]]: All files with content,
            has AUTOWIRE, has PROVIDER annotations
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
            annotation: The annotation string to search for
            
        Returns:
            set[str]: Set of absolute file paths containing the annotation
        """
        cmd = ['grep', '-rlE', f'--include=*{HPP_EXT}', f'--include=*{CPP_EXT}', annotation] + self.filepaths
        result = subprocess.run(cmd, capture_output=True, text=True)
        return {os.path.abspath(f) for f in result.stdout.strip().split('\n') 
               if result.stdout.strip() and f.strip()}
    
    def _scan_file(self, filepath: str) -> tuple[str, bool, bool]:
        """Helper: Scan a single file for annotations.
        
        Args:
            filepath: Path to the file to scan
            
        Returns:
            tuple[str, bool, bool]: File content, has AUTOWIRE, has PROVIDER
        """
        if not filepath.endswith(CPP_EXTENSIONS):
            return (None, False, False)
        try:
            with open(filepath, 'r') as f:
                content = f.read()
                return (content, bool(re.search(AUTOWIRE_PATTERN, content)), bool(re.search(PROVIDER_PATTERN, content)))
        except:
            return (None, False, False)
