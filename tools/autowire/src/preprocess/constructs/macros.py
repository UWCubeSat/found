
from ...common.constants.parser_regex import IDENTIFIER, WS
from ...common.annotations import equals_hash, to_string
from ...common.parsing import parse_delimited_list, is_pos_in_string, get_lowest_operation, OPERATIONS
from typing import List, Optional, Callable, Dict, Union
import re

@equals_hash
class Macro:
    """Base class for all macro-related constructs in C++ preprocessing."""
    def set_parent(self, parent: 'ParameterizedFile') -> 'Macro':
        """Set the parent file for this macro.
        
        Args:
            parent (ParameterizedFile): The file containing this macro
            
        Returns:
            Macro: Self for method chaining
        """
        self.parent = parent
        return self

@to_string
@equals_hash
class DefineDirective(Macro):
    """Represents a #define macro directive with expansion capabilities.
    
    Supports both object-like macros (#define PI 3.14) and function-like macros
    (#define MAX(a,b) ((a)>(b)?(a):(b))). Handles stringification (#) and
    token pasting (##) operators.
    """
    def __init__(self, name: str, definition: str = "", parameters: List[str] = None):
        """Initialize define directive.
        
        Args:
            name (str): Macro name
            definition (str): Macro definition/replacement text
            parameters (List[str], optional): Macro parameter names for function-like macros
        """
        super().__init__()
        self.name = name
        self.definition = definition
        self.parameters = parameters # [] is reserved for function macros with no parameters
        if self.parameters is None:
            self.pattern = rf'\b{re.escape(self.name)}\b'
        else:
            # Whenever we want to match something with parenthesis, we match everything after the
            # begining parenthesis, find the matching closing one, and use that while appending
            # the remainder (after the closing parenthesis) to the rest of the content
            self.pattern = rf'\b{re.escape(self.name)}\s*\(.*'
    
    @classmethod
    def recursive_expand(cls, content: str, macros: Dict[str, 'DefineDirective']) -> str:
        """Recursively expand all macros until no more expansions are possible.
        
        Args:
            content (str): Text content to expand macros in
            macros (Dict[str, DefineDirective]): Available macro definitions
            
        Returns:
            str: Content with all macros expanded
            
        Note:
            Continues expanding until a fixed point is reached (no more changes).
            Handles nested macro invocations and recursive definitions.
        """
        expanded = content
        changed = True
        
        while changed:
            changed = False
            # First expand object-like macros
            for _, macro in macros.items():
                new_expanded = macro.expand(expanded)
                if new_expanded != expanded:
                    expanded = new_expanded
                    changed = True
        return expanded
    
    def expand(self, content: str) -> str:
        """Expand this macro in the given content.
        
        Args:
            content (str): Text content to expand macro in
            
        Returns:
            str: Content with this macro expanded, or original if no matches
        """
        if re.search(self.pattern, content, re.DOTALL):
            # Substitute based on if the macro is a function or not
            return re.sub(
                self.pattern,
                self.definition if self.parameters is None else self.expand_function_macro,
                content,
                flags=re.DOTALL
            )
        # Return original content (no match)
        return content
    
    def expand_function_macro(self, match: re.Match) -> str:
        """Expand a function-like macro invocation.
        
        Args:
            match (re.Match): Regex match object for the macro invocation
            
        Returns:
            str: Expanded macro text with arguments substituted
            
        Note:
            Implements C preprocessor macro expansion rules:
            1. Stringification (#parameter)
            2. Parameter substitution
            3. Token pasting (##)
        """
        full_match = match.group(0)
        # Get all args and keep track of the remainder to append at the end
        args, (_, end) = parse_delimited_list(full_match, level=1)
        remainder = full_match[end+1:]
        
        result = self.definition
        
        # Step 1: Stringify stringified parameters
        result = self.function_macro_step(result,
                                          re.compile(r'(?<!#)#([A-Za-z_]\w*)'),
                                          args=args,
                                          group_number=1,
                                          transformer=lambda quote_char:
                                              f'"{quote_char.replace('\\', '\\\\').replace('"', '\\"')}"')
        # Step 2: Replace all parameters with arguments
        result = self.function_macro_step(result,
                                          re.compile(r'[A-Za-z_]\w*'),
                                          args=args)
        # Step 3: Collapse concatenation operators
        result = self.function_macro_step(result,
                                          re.compile(r"\s*##\s*"),
                                          transformer= lambda _: "")
        
        return result + remainder
    
    def function_macro_step(self,
                            content: str,
                            pattern: re.Pattern,
                            args: List[str] = None,
                            group_number: int = 0,
                            transformer: Optional[Callable] = lambda x: x) -> str:
        """Perform a macro expansion step, ignoring what's inside of strings

        Args:
            content (str): The content to modify
            pattern (re.Pattern): The pattern identifying what to modify
            args (List[str], optional): The arguments. Defaults to None.
            group_number (int, optional): The pattern match number corresponding to the group of interest. Defaults to 0.
            transformer (_type_, optional): How to output the matched group. Defaults to just ouputting as is.

        Returns:
            str: The resulting string
            
        Notes:
            Setting args to None allows you to identify things that are not macro arguments
            and do modifications with them
        """
        result = []
        last_index = 0

        for match in pattern.finditer(content):
            start, end = match.start(), match.end()
            param = match.group(group_number)
            if args is None:
                arg = True
            else:
                arg = None
                
            if param in self.parameters:
                arg = args[self.parameters.index(param)]
        
            if arg and not is_pos_in_string(content, start):
                # Append text before match
                result.append(content[last_index:start])
                # Append stringified argument
                result.append(transformer(arg))
                last_index = end

        result.append(content[last_index:])
        return ''.join(result)

@equals_hash
class IncludeDirective(Macro):
    """Represents a #include directive linking to another preprocessed file."""
    def __init__(self, file: 'ParameterizedFile', external_header: bool = False):
        """Initialize include directive

        Args:
            file (str): The parameterized file for this include macro
            external_header (bool, optional): Whether its an external header. Defaults to False.
        """
        self.file = file
        self.external_header = external_header

class ConditionalDirective(Macro):
    """Represents conditional compilation directives (#if, #ifdef, #ifndef)."""
    def __init__(self, expression: str):
        """Initializes a conditional directive

        Args:
            constants (Tuple[str]): The macros/integers being evaluated
        """
        
        self.expression = expression

    def evaluate(self, macros: Dict[str, DefineDirective]) -> bool:
        """Evaluate the conditional expression using available macros.
        
        Args:
            macros (Dict[str, DefineDirective]): Available macro definitions
            
        Returns:
            bool: True if condition is met, False otherwise
        """
        return bool(MacroExpression.evaluate(self.expression.strip(), macros))

class MacroExpression:
    """Evaluates C preprocessor expressions for conditional compilation.
    
    Handles macro expansion, arithmetic operations, logical operations,
    and the defined() operator according to C preprocessor rules.
    """
    
    @classmethod
    def evaluate(cls, expression: str, macros: Dict[str, DefineDirective]) -> Union[int, bool]:
        """Evaluate a preprocessor expression recursively.
        
        Args:
            expression (str): The expression to evaluate
            macros (Dict[str, DefineDirective]): Available macro definitions
            
        Returns:
            Union[int, bool]: Evaluation result (int for arithmetic, bool for logical)
            
        Note:
            Supports identifiers, defined() operator, unary operators (!,~),
            parentheses, and all C binary operators with proper precedence.
        """
        if re.fullmatch(rf'{IDENTIFIER}', expression):
            if expression in macros:
                return int(DefineDirective.recursive_expand(expression, macros))
            else:
                return 0
        elif re.fullmatch(rf'defined\({WS}{IDENTIFIER}{WS}\)', expression):
            return expression[expression.find('(') + 1:-1].strip() in macros
        elif expression[0] == '!':
            return not cls.evaluate(expression[1:], macros)
        elif expression[0] == '~':
            operand = cls.evaluate(expression[1:], macros)
            assert isinstance(operand, int)
            return ~operand
        elif expression.startswith('(') and expression.endswith(')'):
            return cls.evaluate(expression[1:-1].strip(), macros)
        else:
            operator, position = get_lowest_operation(expression)

            left = cls.evaluate(expression[:position].strip(), macros)
            right = cls.evaluate(expression[position+len(operator):].strip(), macros)
            
            return OPERATIONS[operator](left, right)