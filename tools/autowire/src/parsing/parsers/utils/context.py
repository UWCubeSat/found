"""Enhanced ParseContext with parsing state and utilities."""

import re
from dataclasses import dataclass, field
from typing import Dict, Optional, Union

from ...constructs.definitions.classes import Class
from ...constructs.definitions.functions import Function
from ...constructs.definitions.misc import Namespace, Macro, Typedef, Using, Comment
from ...constructs.definitions.enums import Enum
from ...constructs.statements.variables import Variable


@dataclass
class ParseContext:
    """Unified parsing context with file content, position, symbol tables, and parsing utilities."""
    
    # File content and parsing position
    content: str
    pos: int = 0
    
    # Symbol context (your existing fields)
    current_comment: Optional[Comment] = None
    current_namespace: Optional[Namespace] = None
    macro_table: Dict[str, Macro] = field(default_factory=dict)
    alias_table: Dict[str, Union[Typedef, Using]] = field(default_factory=dict)
    type_definitions: Dict[str, Union[Class, Enum]] = field(default_factory=dict)
    function_definitions: Dict[str, Function] = field(default_factory=dict)
    global_variables: Dict[str, Variable] = field(default_factory=dict)
    static_variables: Dict[str, Variable] = field(default_factory=dict)
    
    # Parsing utilities
    def match_regex(self, pattern: str) -> Optional[str]:
        """Match regex pattern at current position and advance if successful."""
        match = re.match(pattern, self.content[self.pos:])
        if match:
            matched = match.group(0)
            self.pos += len(matched)
            return matched
        return None
    
    def match_compiled_pattern(self, pattern_name: str) -> Optional[re.Match]:
        """Match pre-compiled pattern at current position and advance if successful.
        
        NOTE: This method MUST advance self.pos to match.end() on successful match.
        Parser functions rely on automatic position advancement for clean parsing logic.
        """
        from ....common.constants.parser_regex import match_pattern
        match = match_pattern(pattern_name, self.content, self.pos)
        if match:
            self.pos = match.end()  # CRITICAL: Advance position on success
            return match
        return None
    
    def extract_captures(self, pattern_name: str, match: re.Match) -> Dict[str, Optional[str]]:
        """Extract named capture groups from match using predefined indices."""
        from ....common.constants.parser_regex import CAPTURE_GROUPS
        captures = {}
        group_map = CAPTURE_GROUPS.get(pattern_name, {})
        
        for name, index in group_map.items():
            try:
                captures[name] = match.group(index)
            except IndexError:
                captures[name] = None
        
        return captures
    
    def parse_identifier(self) -> Optional[str]:
        """Parse C++ identifier at current position."""
        return self.match_regex(r'[a-zA-Z_]\w*')
    
    def skip_whitespace(self):
        """Skip whitespace and comments at current position."""
        from ....common.constants.parser_regex import WS
        self.match_regex(WS)
    
    def peek(self, offset: int = 0) -> str:
        """Look ahead at character without advancing position."""
        pos = self.pos + offset
        return self.content[pos] if pos < len(self.content) else ''
    
    def peek_word(self) -> str:
        """Look ahead at next word without advancing position."""
        match = re.match(r'\s*(\w+)', self.content[self.pos:])
        return match.group(1) if match else ''
    
    def save_position(self) -> int:
        """Save current parsing position for backtracking."""
        return self.pos
    
    def restore_position(self, saved_pos: int):
        """Restore parsing position for backtracking."""
        self.pos = saved_pos
    
    def at_end(self) -> bool:
        """Check if at end of content."""
        return self.pos >= len(self.content)
    
    def remaining_content(self) -> str:
        """Get remaining unparsed content."""
        return self.content[self.pos:]
    
    def find_matching_brace(self, open_char: str = '{', close_char: str = '}') -> int:
        """Find position of matching closing brace, returns -1 if not found."""
        if self.peek() != open_char:
            return -1
        
        depth = 0
        pos = self.pos
        while pos < len(self.content):
            char = self.content[pos]
            if char == open_char:
                depth += 1
            elif char == close_char:
                depth -= 1
                if depth == 0:
                    return pos
            pos += 1
        return -1
    
    def extract_braced_content(self) -> Optional[str]:
        """Extract content between matching braces, including the braces."""
        start_pos = self.pos
        end_pos = self.find_matching_brace()
        if end_pos == -1:
            return None
        
        content = self.content[start_pos:end_pos + 1]
        self.pos = end_pos + 1
        return content
    
    def add_context(self, other: 'ParseContext') -> None:
        """Merge symbol tables from another parsing context.
        
        Args:
            other (ParseContext): Context to merge symbols from
            
        Note:
            Updates all symbol tables with definitions from the other context.
            Used when processing included files or merging translation units.
        """
        self.macro_table.update(other.macro_table)
        self.alias_table.update(other.alias_table)
        self.type_definitions.update(other.type_definitions)
        self.function_definitions.update(other.function_definitions)
        self.global_variables.update(other.global_variables)
        self.static_variables.update(other.static_variables)