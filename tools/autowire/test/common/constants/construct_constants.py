"""Test constants for construct testing."""

# =============================================================================
# FILE PATHS
# =============================================================================
SAMPLE_FILE_PATH = "/home/user/project/src/main.cpp"
HEADER_FILE_PATH = "/home/user/project/include/header.h"

# =============================================================================
# BASIC IDENTIFIERS
# =============================================================================
CLASS_NAME = "MyClass"
BASE_CLASS_NAME = "BaseClass"
DERIVED_CLASS_NAME = "DerivedClass"
FUNCTION_NAME = "processData"
VARIABLE_NAME = "counter"
PARAMETER_NAME = "param"
ENUM_NAME = "Status"
NAMESPACE_NAME = "utils"
TYPEDEF_NAME = "StringMap"
MACRO_NAME = "MAX_SIZE"

# =============================================================================
# TYPE NAMES
# =============================================================================
INT_TYPE = "int"
STRING_TYPE = "std::string"
VECTOR_TYPE = "std::vector"
UNIQUE_PTR_TYPE = "std::unique_ptr"

# =============================================================================
# ACCESS LEVELS
# =============================================================================
PUBLIC_ACCESS = "public"
PRIVATE_ACCESS = "private"
PROTECTED_ACCESS = "protected"

# =============================================================================
# EXPRESSIONS AND CODE SNIPPETS
# =============================================================================
SIMPLE_EXPRESSION = "42"
FUNCTION_CALL_EXPRESSION = "getValue()"
INITIALIZATION_LIST = "{1, 2, 3}"
CONDITION_EXPRESSION = "x > 0"
LOOP_INIT = "int i = 0"
LOOP_CONDITION = "i < 10"
LOOP_INCREMENT = "++i"
SIMPLE_CODE = "int x = 42;"
ASSIGNMENT_CODE = "counter += 1;"

# =============================================================================
# COMMENTS
# =============================================================================
SINGLE_LINE_COMMENT = "// This is a comment"
MULTI_LINE_COMMENT = "/* Multi-line\n   comment */"
AUTOWIRE_COMMENT = "// [[AUTOWIRE]]"

# =============================================================================
# TEMPLATE AND GENERIC PARAMETERS
# =============================================================================
TEMPLATE_PARAM_T = "T"
TEMPLATE_PARAM_U = "U"

# =============================================================================
# COLLECTIONS AND LISTS
# =============================================================================
ENUM_VALUES = ["SUCCESS", "ERROR", "PENDING"]
MACRO_WITH_PARAMS = ["x", "y"]

# =============================================================================
# MACRO AND PREPROCESSOR
# =============================================================================
MACRO_DEFINITION = "1024"
SYSTEM_INCLUDE = "<iostream>"
LOCAL_INCLUDE = "\"myheader.h\""

# =============================================================================
# USING DECLARATIONS
# =============================================================================
USING_NAMESPACE = "std"
USING_TYPE = "std::string"

# =============================================================================
# NUMERIC CONSTANTS
# =============================================================================
LINE_NUMBER_10 = 10
LINE_NUMBER_15 = 15
LINE_NUMBER_20 = 20
POINTER_LEVEL_1 = 1
POINTER_LEVEL_2 = 2
POINTER_LEVEL_3 = 3
EXPECTED_COUNT_0 = 0
EXPECTED_COUNT_1 = 1
EXPECTED_COUNT_2 = 2
EXPECTED_COUNT_3 = 3

# =============================================================================
# BOOLEAN CONSTANTS
# =============================================================================
EXPECTED_TRUE = True
EXPECTED_FALSE = False