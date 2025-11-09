"""Constants for discovery operations.

This module defines regex patterns and file extensions used throughout
the autowire discovery system for identifying annotations and C++ files.
"""

# Regex pattern for AUTOWIRE annotations with optional parameters
# Matches: [[AUTOWIRE]], [[AUTOWIRE()]], [[AUTOWIRE(param)]]
# Allows flexible whitespace around brackets and keywords
AUTOWIRE_PATTERN = r"\s*\[\[\s*AUTOWIRE\s*(\([^)]*\))?\s*\]\]\s*"

# Regex pattern for PROVIDER annotations with optional parameters  
# Matches: [[PROVIDER]], [[PROVIDER()]], [[PROVIDER(param)]]
# Allows flexible whitespace around brackets and keywords
PROVIDER_PATTERN = r"\s*\[\[\s*PROVIDER\s*(\([^)]*\))?\s*\]\]\s*"

# C++ header file extension
HPP_EXT = ".hpp"

# C++ source file extension
CPP_EXT = ".cpp"

# Tuple of all supported C++ file extensions
CPP_EXTENSIONS = (HPP_EXT, CPP_EXT)