"""File discovery for C++ source files."""


from .project_cache import ProjectFileCache, FileInfo
from .annotation_scanner import AnnotationScanner


class FileDiscovery:
    """Discovers C++ files and scans for AUTOWIRE/PROVIDER annotations."""
    
    def __init__(self, root_path: str):
        """Initialize with project root path.
        
        Args:
            root_path: Root directory path to scan for C++ files
        """
        self.root_path = root_path
        self.scanner = AnnotationScanner([root_path])
    
    def get_annotated_files(self) -> ProjectFileCache:
        """Scan for C++ files and build project cache.
        
        Returns:
            ProjectFileCache: Cache containing all discovered files with annotation info,
            plus lists of files with AUTOWIRE and PROVIDER annotations
        """
        scan_results = self.scanner.scan()
        
        all_files = {}
        autowire_files = []
        provider_files = []
        
        for filepath, (content, has_autowire, has_provider) in scan_results.items():
            if content is None:
                with open(filepath, 'r') as f:
                    content = f.read()
            
            file_info = FileInfo(
                file_path=filepath,
                file_content=content,
                has_autowire=has_autowire,
                has_provider=has_provider
            )
            
            all_files[filepath] = file_info
            
            if has_autowire:
                autowire_files.append(filepath)
            if has_provider:
                provider_files.append(filepath)
        
        return ProjectFileCache(all_files, autowire_files, provider_files)