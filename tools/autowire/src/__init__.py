"""Autowire: C++ dependency injection code generator.

Autowire is a C++ dependency injection generator that automatically injects
dependencies into objects while autowiring unknown dependencies to command-line
arguments. It processes C++ source files with [[AUTOWIRE]] and [[PROVIDER]]
annotations to generate factory functions and CLI parsers.

Main modules:
- discovery: File scanning and annotation detection
- preprocess: C++ preprocessing with macro expansion
- parsing: C++ code parsing and construct representation
- analysis: Dependency graph analysis and validation
- codegen: C++ code generation for dependency injection
"""

import sys
from pathlib import Path

# Add the 'src' directory to sys.path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "src"))
