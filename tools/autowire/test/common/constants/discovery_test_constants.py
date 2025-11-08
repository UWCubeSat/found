"""Test constants for autowire test suite."""

# File paths and names
TEST_AUTOWIRE_FILE = "autowire.cpp"
TEST_PROVIDER_FILE = "provider.hpp"
TEST_BOTH_FILE = "both.cpp"
TEST_NONE_FILE = "none.cpp"
TEST_PLAIN_FILE = "plain.cpp"
TEST_WHITESPACE_FILE = "whitespace.cpp"
TEST_TXT_FILE = "readme.txt"

# Test file contents
AUTOWIRE_CONTENT ="""
// [[AUTOWIRE]]
class AutowiredClass {
public:
    void method();
};"""

PROVIDER_CONTENT = """
// [[PROVIDER]]
class ServiceProvider {
public:
    void provide();
};"""

BOTH_CONTENT = """
// [[AUTOWIRE]]
// [[PROVIDER]]
class BothClass {
public:
    BothClass() = default;
};"""

PLAIN_CONTENT = """
class PlainClass {
public:
    void method();
    int getValue() const;
private:
    int value_;
};"""

NONE_CONTENT = """
class Plain {
public:
    Plain() = default;
};"""

WHITESPACE_CONTENT = """
//  [[ AUTOWIRE ]]  
//  [[ PROVIDER ( service ) ]]  
class Test {
public:
    void testMethod();
};"""

TXT_CONTENT = "// [[AUTOWIRE]] in text file"

# Simple annotation patterns
SIMPLE_AUTOWIRE = "// [[AUTOWIRE]]"
SIMPLE_PROVIDER = "// [[PROVIDER]]"

# Test paths
TEST_ROOT_PATH = "/test/path"
TEST_NONEXISTENT_PATH = "/nonexistent/file.cpp"
TEST_ABSOLUTE_PATH = "/absolute/path/to/file.cpp"
TEST_RELATIVE_PATH = "relative/path/file.cpp"
TEST_EMPTY_PATH = ""

# Mock grep command
MOCK_GREP_PATH = "/usr/bin/grep"

# File extensions for testing
TEST_CPP_EXTENSIONS = (".cpp", ".hpp")

# Annotation test cases
AUTOWIRE_VALID_CASES = [
    "[[AUTOWIRE]]",
    "  [[AUTOWIRE]]  ",
    "[[ AUTOWIRE ]]",
    "// [[AUTOWIRE]]",
    "[[AUTOWIRE()]]",
    "[[AUTOWIRE(MyService)]]",
    "[[AUTOWIRE(param1, param2)]]",
    "[[ AUTOWIRE ( service ) ]]",
    "  [[ AUTOWIRE(complex::Type<T>) ]]  "
]

PROVIDER_VALID_CASES = [
    "[[PROVIDER]]",
    "  [[PROVIDER]]  ",
    "[[ PROVIDER ]]",
    "// [[PROVIDER]]",
    "[[PROVIDER()]]",
    "[[PROVIDER(singleton)]]",
    "[[PROVIDER(scope=request)]]",
    "[[ PROVIDER ( factory ) ]]",
    "  [[ PROVIDER(MyFactory<T>) ]]  "
]

INVALID_ANNOTATION_CASES = [
    "[AUTOWIRE]",      # Single brackets
    "[[AUTOWIRE]",     # Missing closing bracket
    "[AUTOWIRE]]",     # Missing opening bracket
    "((AUTOWIRE))",    # Wrong bracket type
    "[[AUTOWIRE(",     # Unclosed parameter
    "[[AUTOWIRE)]",    # Wrong parameter brackets
    "AUTOWIRE",        # No brackets at all
    "[[AUTO WIRE]]"    # Space in keyword
]

WHITESPACE_TEST_CASES = [
    "[[AUTOWIRE]]",
    " [[AUTOWIRE]] ",
    "  [[  AUTOWIRE  ]]  ",
    "\t[[\tAUTOWIRE\t]]\t",
    "\n[[\nAUTOWIRE\n]]\n",
    "[[AUTOWIRE()]]",
    "[[ AUTOWIRE () ]]",
    "[[  AUTOWIRE  (  param  )  ]]"
]

COMPLEX_PARAMETER_CASES = [
    "[[AUTOWIRE(std::shared_ptr<DatabaseService>)]]",
    "[[PROVIDER(factory=MyFactory, scope=singleton)]]",
    "[[AUTOWIRE(Service1, Service2, Service3)]]",
    "[[PROVIDER(name=\"my-service\", version=1.0)]]"
]

CODE_CONTEXT_CASES = [
    ("// [[AUTOWIRE]]\nclass MyClass {};", "AUTOWIRE"),
    ("/*\n * [[PROVIDER]]\n */\nclass Service {};", "PROVIDER"),
    ("class Test { /* [[AUTOWIRE]] */ };", "AUTOWIRE"),
    ("// [[AUTOWIRE(DatabaseService)]]\nclass Controller {};", "AUTOWIRE")
]

CASE_SENSITIVE_INVALID = [
    "[[autowire]]",
    "[[Autowire]]",
    "[[provider]]",
    "[[Provider]]"
]

# Mock file paths for testing
MOCK_AUTOWIRE_PATH = "/path/autowire.cpp"
MOCK_PROVIDER_PATH = "/path/provider.cpp"
MOCK_BOTH_PATH = "/path/both.cpp"
MOCK_NONE_PATH = "/path/none.cpp"
MOCK_NEW_AUTOWIRE_PATH = "/path/new_autowire.cpp"
MOCK_NEW_PROVIDER_PATH = "/path/new_provider.cpp"
MOCK_NEW_BOTH_PATH = "/path/new_both.cpp"
MOCK_NEW_NONE_PATH = "/path/new_none.cpp"

# Mock file contents for testing
MOCK_AUTOWIRE_CONTENT = "// [[AUTOWIRE]]"
MOCK_PROVIDER_CONTENT = "// [[PROVIDER]]"
MOCK_BOTH_CONTENT = "// [[AUTOWIRE]] [[PROVIDER]]"
MOCK_NONE_CONTENT = "// no annotations"
MOCK_UPDATED_CONTENT = "// updated content"

# Expected results for file discovery tests
EXPECTED_BASIC_MOCK_RESULTS = {
    "autowire": ("// [[AUTOWIRE]]\nclass AutowiredClass {};", True, False),
    "provider": ("// [[PROVIDER]]\nclass ServiceProvider {};", False, True),
    "both": ("// [[AUTOWIRE]]\n// [[PROVIDER]]\nclass BothClass {};", True, True),
    "plain": ("class PlainClass {};", False, False)
}

EXPECTED_GREP_MOCK_RESULTS = {
    "autowire": (None, True, False),
    "provider": (None, False, True)
}

EXPECTED_MIXED_MOCK_RESULTS = {
    "autowire": (None, True, False),
    "provider": ("// [[PROVIDER]]\nclass ServiceProvider {};", False, True),
    "plain": ("class PlainClass {};", False, False)
}

# Helper functions for creating test objects
def create_sample_file_info_dict():
    """Create a dictionary of sample FileInfo objects for testing."""
    from src.discovery.project_cache import FileInfo
    return {
        MOCK_AUTOWIRE_PATH: FileInfo(MOCK_AUTOWIRE_PATH, MOCK_AUTOWIRE_CONTENT, True, False),
        MOCK_PROVIDER_PATH: FileInfo(MOCK_PROVIDER_PATH, MOCK_PROVIDER_CONTENT, False, True),
        MOCK_BOTH_PATH: FileInfo(MOCK_BOTH_PATH, MOCK_BOTH_CONTENT, True, True),
        MOCK_NONE_PATH: FileInfo(MOCK_NONE_PATH, MOCK_NONE_CONTENT, False, False)
    }

def create_sample_project_cache():
    """Create a sample ProjectFileCache for testing."""
    from src.discovery.project_cache import ProjectFileCache
    sample_files = create_sample_file_info_dict()
    return ProjectFileCache(
        all_files=sample_files,
        autowire_files=[MOCK_AUTOWIRE_PATH, MOCK_BOTH_PATH],
        provider_files=[MOCK_PROVIDER_PATH, MOCK_BOTH_PATH]
    )

def build_mock_results_dict(file_mapping, results_template):
    """Build mock results dictionary using file mapping and template."""
    return {file_mapping[key]: results_template[key] for key in results_template}

def create_temp_directory():
    """Create temporary directory for tests."""
    import tempfile
    return tempfile.mkdtemp()

def cleanup_temp_directory(temp_dir):
    """Clean up temporary directory after tests."""
    import shutil
    shutil.rmtree(temp_dir)

def create_test_file(directory, filename, content):
    """Create a test file with given content."""
    import os
    file_path = os.path.join(directory, filename)
    with open(file_path, 'w') as f:
        f.write(content)
    return file_path

def create_standard_test_files(temp_dir, subdir=None):
    """Create standard test files (autowire, provider, both, none, txt)."""
    import os
    target_dir = os.path.join(temp_dir, subdir) if subdir else temp_dir
    if subdir:
        os.makedirs(target_dir)
    
    files = {}
    files['autowire'] = create_test_file(target_dir, TEST_AUTOWIRE_FILE, AUTOWIRE_CONTENT)
    files['provider'] = create_test_file(target_dir, TEST_PROVIDER_FILE, PROVIDER_CONTENT)
    files['both'] = create_test_file(target_dir, TEST_BOTH_FILE, BOTH_CONTENT)
    files['none'] = create_test_file(target_dir, TEST_NONE_FILE, NONE_CONTENT)
    files['txt'] = create_test_file(target_dir, TEST_TXT_FILE, TXT_CONTENT)
    return files

def create_multi_test_files(temp_dir):
    """Create multiple test files for multi-file scenarios."""
    multi_files = {}
    for key, filename in MULTI_FILE_NAMES.items():
        multi_files[key] = create_test_file(temp_dir, filename, MULTI_FILE_CONTENTS[key])
    return multi_files

# Multi-file test scenarios
MULTI_FILE_NAMES = {
    "service1": "database_service.cpp",
    "service2": "auth_service.hpp", 
    "controller1": "user_controller.cpp",
    "controller2": "api_controller.hpp",
    "util": "logger.cpp",
    "plain": "helper.cpp"
}

MULTI_FILE_CONTENTS = {
    "service1": "// [[PROVIDER]]\nclass DatabaseService { void connect(); };",
    "service2": "// [[PROVIDER]]\nclass AuthService { bool authenticate(); };",
    "controller1": "// [[AUTOWIRE]]\nclass UserController { DatabaseService* db; };",
    "controller2": "// [[AUTOWIRE]]\nclass ApiController { AuthService* auth; };",
    "util": "// [[AUTOWIRE]]\n// [[PROVIDER]]\nclass Logger { void log(); };",
    "plain": "class Helper { static void utility(); };"
}

MULTI_FILE_ANNOTATIONS = {
    "service1": (False, True),   # Provider only
    "service2": (False, True),   # Provider only  
    "controller1": (True, False), # Autowire only
    "controller2": (True, False), # Autowire only
    "util": (True, True),        # Both
    "plain": (False, False)      # Neither
}

EXPECTED_MULTI_FILE_RESULTS = {
    key: (MULTI_FILE_CONTENTS[key], *MULTI_FILE_ANNOTATIONS[key]) 
    for key in MULTI_FILE_NAMES
}

# Common test literals
TEST_CONTENT_LITERAL = "content"
TEST_MOCK_LITERAL = "test"
WHITESPACE_FILENAME = "whitespace.cpp"
SRC_SUBDIR = "src"
AUTOWIRE_PATTERN_LITERAL = "AUTOWIRE_PATTERN"
GREP_PATH_LITERAL = "/usr/bin/grep"

# Expected file contents for basic tests
EXPECTED_AUTOWIRE_CLASS = "// [[AUTOWIRE]]\nclass AutowiredClass {};"
EXPECTED_PROVIDER_CLASS = "// [[PROVIDER]]\nclass ServiceProvider {};"
EXPECTED_BOTH_CLASS = "// [[AUTOWIRE]]\n// [[PROVIDER]]\nclass BothClass {};"
EXPECTED_PLAIN_CLASS = NONE_CONTENT

# Multi-file test keys
MULTI_FILE_AUTOWIRE_KEYS = ["controller1", "controller2", "util"]
MULTI_FILE_PROVIDER_KEYS = ["service1", "service2", "util"]

# Common test tuples
NONE_FALSE_FALSE = (None, False, False)
TRUE_FALSE_TUPLE = (True, False)
FALSE_TRUE_TUPLE = (False, True)
TRUE_TRUE_TUPLE = (True, True)
FALSE_FALSE_TUPLE = (False, False)

# Mock return values
MOCK_GREP_RETURN = {"test": (None, True, False)}
MOCK_SLOW_RETURN = {"test": ("content", True, False)}
EMPTY_CACHE_ARGS = {"all_files": {}, "autowire_files": [], "provider_files": []}

# File read error mock
FILE_READ_ERROR_MOCK = {TEST_NONEXISTENT_PATH: (None, True, False)}