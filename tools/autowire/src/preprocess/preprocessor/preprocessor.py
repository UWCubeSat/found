"""C++ preprocessor for macro expansion and conditional compilation."""

from copy import deepcopy
import re
import os
import logging

from ..constructs.macros import DefineDirective, ConditionalDirective, IncludeDirective
from ...discovery import ProjectFileCache, FileInfo
from ..constructs.parameterized_file import ParameterizedFile
from .parameterized_file_cache import ParameterizedFileCache

from typing import List, Dict, Set

class Preprocessor:
    """Handles C++ preprocessing including macro expansion, conditional compilation, and include resolution."""
    def __init__(self, project_file_cache: ProjectFileCache, include_paths: List[str] = None):
        """Initialize preprocessor with file cache and include paths.
        
        Args:
            project_file_cache (ProjectFileCache): Cache of project files
            include_paths (List[str], optional): Additional include search paths
        """
        self.project_file_cache = project_file_cache
        base_paths = include_paths or [self.project_file_cache._root_path]
        self.include_paths = []
        while base_paths:
            path = base_paths.pop()
            self.include_paths.append(path)
            for root, dirs, _ in os.walk(path):
                for dir_name in dirs:
                    base_paths.append(os.path.join(root, dir_name))
    
    def get_translation_unit(self, file_path: str) -> ParameterizedFileCache:
        """Get complete translation unit with all dependencies preprocessed.
        
        Args:
            file_path (str): Path to the main source file
            
        Returns:
            ParameterizedFileCache: Cache containing preprocessed file and all dependencies
        """
        cache = ParameterizedFileCache()
        self.get_file(file_path, cache)
        
        return cache
    
    def get_file(self, file_path: str, cache: ParameterizedFileCache, macros: Dict[str, DefineDirective] = {}, processed_files: Set[str] = set()) -> ParameterizedFile:
        """Get preprocessed file with macro context, handling includes and circular dependencies.
        
        Args:
            file_path (str): Path to the file to preprocess
            cache (ParameterizedFileCache): Cache for storing processed files
            macros (Dict[str, DefineDirective], optional): Active macro definitions
            processed_files (Set[str], optional): Set of already processed files for circular detection
            
        Returns:
            ParameterizedFile: Preprocessed file with expanded macros and resolved includes
        """
        # Step 1: Get the file content and reduce the macros
        file_path, file_content = self.fetch_file_content(file_path)
        macros = self.reduce_macros(file_content, macros)
        
        # Step 2: If the file with the macros already exists in cache, return it
        parameterized_file = cache.get_file(file_path, macros)
        if parameterized_file:
            return parameterized_file
        
        # Step 3: Get the preprocessed file, add it to the cache, and return it
        processed_files.add(file_path)
        file = self.preprocess_file(file_path, file_content, cache, macros, processed_files)
        cache.put_file(file, list(macros.values()))
        return file
    
    def preprocess_file(self, file_path: str, raw_content: str, cache: ParameterizedFileCache, macros: Dict[str, DefineDirective] = None, processed_files: Set[str] = set()) -> ParameterizedFile:
        """Apply conditional compilation and macro expansion."""
        lines = self.get_raw_lines(raw_content)
        result = []
        condition_stack = []
        active_macros = deepcopy(macros) if macros else {}
        active_lines = []
        last_macro = None
        includes = []
        global_include = True

        def eval_conditions():
            return all(all(x) for x in condition_stack)

        i = 0
        while i != len(lines):
            line = lines[i]
            stripped = line.strip()

            if stripped.startswith('#'):
                if eval_conditions():
                    result.append(DefineDirective.recursive_expand('\n'.join(active_lines), active_macros))
                    active_lines = []

                if stripped.startswith('#if') or stripped.startswith('#elif'):
                    stack = []
                    if line.startswith('#elif'):
                        # Take the stack out and negate the previous condition
                        stack = condition_stack.pop()
                        stack.append(not stack.pop())
                    # Add our new condition
                    expression = stripped.split(maxsplit=1)
                    if len(expression) != 2:
                        raise ValueError(f"The line {line} is an invalid preprocessor line")
                    expression = expression[1]
                    if line.startswith('#ifdef'):
                        expression = f"defined({expression})"
                    elif line.startswith('#ifndef'):
                        expression = f"!defined({expression})"
                    condition = ConditionalDirective(expression).evaluate(active_macros)
                    stack.append(condition)
                    condition_stack.append(stack)
                elif stripped.startswith('#else'):
                    condition_stack[-1][-1] = not condition_stack[-1][-1]
                elif stripped.startswith('#endif'):
                    condition_stack.pop()
                elif stripped.startswith('#define'):
                    if eval_conditions():
                        if match := re.match(r'#define\s+(\w+)(\([^)]*\))?(?:\s+(.*))?', stripped):
                            macro_name = match.group(1)
                            params_str = match.group(2)
                            body = match.group(3) if match.group(3) else ""

                            if params_str:
                                params = [p.strip() for p in params_str[1:-1].split(',') if p.strip()]
                                macro = DefineDirective(macro_name, body, params)
                            else:
                                macro = DefineDirective(macro_name, body)

                            if macro_name in active_macros:
                                raise ValueError(f"Duplicate macro {macro_name}")

                            active_macros[macro_name] = macro
                            last_macro = macro_name
                    # Don't add #define lines to result
                elif stripped.startswith('#undef'):
                    if eval_conditions():
                        parts = stripped.split()
                        if len(parts) >= 2:
                            macro_name = parts[1]
                            active_macros.pop(macro_name)
                        else:
                            active_macros.pop(last_macro)
                    # Don't add #undef lines to result
                elif stripped.startswith('#include'):
                    include_directive = stripped.split(maxsplit=1)
                    if not len(include_directive) == 2:
                        raise ValueError(f"The line {line} is not a preprocessor line")
                    include_file_path = include_directive[1].strip()
                    external_header = include_file_path[0] == '<'
                    include_file_path = include_file_path[1:-1]
                    if not external_header:
                            # Here, we handle circular includes by ignoring it in the current translation unit (as the preprocessor actually does).
                            if include_file_path not in processed_files:
                                included_file = self.get_file(include_file_path, cache, active_macros)
                                active_macros.update(included_file.defined_macros)
                                # If its at the beginning, add it to be a dependency file,
                                # otherwise, copy its contents in
                                if global_include:
                                    includes.append(IncludeDirective(included_file, external_header))
                                else:
                                    lines[i:i+1] = self.inline_include(line, included_file.raw_content)
                                    i -= 1
                            else:
                                logging.info(f"Preprocessor: Skipping circular inclusion {line} in file {file_path}")
                    else:
                        logging.info(f"Preprocessor: Ignoring external header line {line}")                   
                else:
                    # Ignoring #pragma and other obscure macros
                    logging.info(f"Preprocessor: Ignoring preprocessor line {line}")
            else:
                if eval_conditions():
                    if stripped:
                        global_include = False
                    active_lines.append(line)
            
            i += 1

        if eval_conditions() and active_lines:
            result.append(DefineDirective.recursive_expand('\n'.join(active_lines), active_macros))
            active_lines = []

        # Return result without additional macro expansion (already done per line)
        return ParameterizedFile(file_path, '\n'.join(result).strip(), active_macros, includes)

    def get_raw_lines(self, raw_content: str) -> List[str]:
        """Join lines that end with backslash continuation."""
        lines = raw_content.split('\n')
        result = []
        i = 0
        while i < len(lines):
            line = lines[i]
            # Check for line continuation (backslash at end)
            while line.rstrip().endswith('\\') and i + 1 < len(lines):
                # Remove backslash and join with next line
                line = line.rstrip()[:-1] + ' ' + lines[i + 1].lstrip()
                i += 1
            result.append(line)
            i += 1
        return result
        
    def fetch_file_content(self, file_path: str) -> tuple[str, str]:
        """Fetch file content from cache or filesystem using include search paths.
        
        Args:
            file_path (str): Path to the file to fetch
            
        Returns:
            tuple[str, str]: (resolved_path, file_content)
            
        Raises:
            ValueError: If file cannot be found in any search path
        """
        possible_paths = [file_path] + [os.path.abspath(os.path.join(i_path, file_path)) for i_path in self.include_paths]
        for path in possible_paths:
            if path in self.project_file_cache:
                pass
            elif os.path.isfile(file_path):
                self.project_file_cache.add_file(FileInfo.get_file(file_path))
            else:
                continue
            return path, self.project_file_cache.get_file_content(path)
        
        raise ValueError(f"Path {file_path} does not exist")
    
    def reduce_macros(self, file_content: str, macros: Dict[str, DefineDirective]) -> Dict[str, DefineDirective]:
        """Remove unused macros from macro context to optimize processing.
        
        Args:
            file_content (str): Content of the file being processed
            macros (Dict[str, DefineDirective]): Current macro definitions
            
        Returns:
            Dict[str, DefineDirective]: Filtered macros that are actually used in the file
        """
        macros = deepcopy(macros)
        
        to_remove = set()
        for macro in macros:
            if not re.search(rf"\b{macro}\b", file_content):
                to_remove.add(macro)
        for macro in to_remove:
            del macros[macro]
        
        return macros
    
    def inline_include(self, include_line: str, content: str) -> List[str]:
        """Inline include content while preserving the original line's indentation.
        
        Args:
            include_line (str): The original #include line with its indentation
            content (str): Content to inline
            
        Returns:
            List[str]: Content lines with preserved indentation
        """
        indent = include_line[:len(include_line) - len(include_line.lstrip())]
        included_lines = self.get_raw_lines(content)
        return [indent + line if line.strip() else line for line in included_lines]