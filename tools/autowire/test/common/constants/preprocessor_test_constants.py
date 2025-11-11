"""Test constants for preprocessor test suite."""

from src.preprocess.constructs.macros import DefineDirective

# =============================================================================
# HELPER FUNCTIONS
# =============================================================================
def to_define_string(define_directive):
    """Convert DefineDirective to #define string."""
    if define_directive.parameters is None:
        return f"#define {define_directive.name} {define_directive.definition}"
    elif define_directive.parameters == []:
        return f"#define {define_directive.name}() {define_directive.definition}"
    else:
        params = ", ".join(define_directive.parameters)
        return f"#define {define_directive.name}({params}) {define_directive.definition}"

def to_multiline_define_string(define_directive):
    """Convert DefineDirective to multi-line #define string with line continuations."""
    definition = define_directive.definition.replace("  ", " \\\n    ")
    if define_directive.parameters is None:
        return f"#define {define_directive.name} {definition}"
    elif define_directive.parameters == []:
        return f"#define {define_directive.name}() {definition}"
    else:
        params = ", ".join(define_directive.parameters)
        return f"#define {define_directive.name}({params}) \\\n{definition}"

# =============================================================================
# BASIC MACRO DEFINES
# =============================================================================
# Object-like macros
EMPTY_DEFINE = DefineDirective("EMPTY", "")
PLACEHOLDER_DEFINE = DefineDirective("PLACEHOLDER", "")
PI_DEFINE = DefineDirective("PI", "3.14159")
VERSION_DEFINE = DefineDirective("VERSION", "42")
ANOTHER_DEFINE = DefineDirective("ANOTHER", "24")
TEMP_DEFINE = DefineDirective("TEMP", "200")

# Function-like macros (no parameters)
INIT_DEFINE = DefineDirective("INIT", "{ initialized = true; }", [])
CLEANUP_DEFINE = DefineDirective("CLEANUP", "{ cleanup_done = true; }", [])

# Function-like macros (with parameters)
MAX_DEFINE = DefineDirective("MAX", "((a) > (b) ? (a) : (b))", ["a", "b"])
MIN_DEFINE = DefineDirective("MIN", "((a) < (b) ? (a) : (b))", ["a", "b"])
ADD_DEFINE = DefineDirective("ADD", "(a + b)", ["a", "b"])
DIVIDE_DEFINE = DefineDirective("DIVIDE", "(a / b)", ["a", "b"])
FUNC_DEFINE = DefineDirective("FUNC", "ret name(arg1 a, arg2 b) { return a + b; }", ["name", "ret", "arg1", "arg2"])

# =============================================================================
# TOKEN MANIPULATION DEFINES
# =============================================================================
# Stringification
STR_DEFINE = DefineDirective("STR", "#x", ["x"])
QUOTE_DEFINE = DefineDirective("QUOTE", "#text", ["text"])
STRINGIFY_DEFINE = DefineDirective("STRINGIFY", "#x", ["x"])
QUOTE_PARAM_DEFINE = DefineDirective("QUOTE_PARAM", "\"x\"", ["x"])

# Token pasting
CONCAT_DEFINE = DefineDirective("CONCAT", "a##b", ["a", "b"])
PREFIX_DEFINE = DefineDirective("PREFIX", "prefix_##name", ["name"])
SUFFIX_DEFINE = DefineDirective("SUFFIX", "name##_suffix", ["name"])
MAKE_VAR_DEFINE = DefineDirective("MAKE_VAR", "prefix##_##suffix", ["prefix", "suffix"])

# Mixed operations
MIXED1_DEFINE = DefineDirective("MIXED1", "prefix_#x##_suffix", ["x"])

# =============================================================================
# MULTI-LINE MACRO DEFINES
# =============================================================================
# Line continuation macros
LONG_MACRO_DEFINE = DefineDirective("LONG_MACRO", "do {  printf(\"%s\", x);  } while(0)", ["x"])
MAX_LINE_CONT_DEFINE = DefineDirective("MAX", "((a) > (b) ?  (a) : (b))", ["a", "b"])
COMPLEX_DEFINE = DefineDirective("COMPLEX", "if (x > 0) {  return x * 2;  }", ["x"])

# Nested macro definitions
MAKE_GETTER_DEFINE = DefineDirective("MAKE_GETTER", "CONCAT(get, name)", ["type", "name"])
MAKE_SETTER_DEFINE = DefineDirective("MAKE_SETTER", "CONCAT(set, name)", ["type", "name"])
PROPERTY_DEFINE = DefineDirective("PROPERTY", "private:  type m_##name;  public:  type MAKE_GETTER(type, name)() const { return m_##name; }  void MAKE_SETTER(type, name)(const type& value) { m_##name = value; }", ["type", "name"])
SMART_POINTER_DEFINE = DefineDirective("SMART_POINTER", "std::shared_ptr<type> type##Ptr;  std::weak_ptr<type> type##WeakPtr;  std::unique_ptr<type> type##UniquePtr;", ["type"])

# =============================================================================
# CONDITIONAL COMPILATION DEFINES
# =============================================================================
# Logging macros
LOG_DEBUG_DEFINE = DefineDirective("LOG", "printf(x)", ["x"])
LOG_EMPTY_DEFINE = DefineDirective("LOG", "", ["x"])
LOG_DEBUG_MSG_DEFINE = DefineDirective("LOG", "std::cout << \"[DEBUG] \" << msg << std::endl;", ["msg"])
LOG_NO_OP_DEFINE = DefineDirective("LOG", "// no-op", ["msg"])

# Mode macros
MODE_AB_DEFINE = DefineDirective("MODE", "\"AB\"")
MODE_A_DEFINE = DefineDirective("MODE", "\"A\"")
MODE_NONE_DEFINE = DefineDirective("MODE", "\"NONE\"")

# Validation macros
VALIDATE_INPUT_DEFINE = DefineDirective("VALIDATE_INPUT", "assert(condition && message);", ["condition", "message"])
VALIDATE_INPUT_MULTILINE_DEFINE = DefineDirective("VALIDATE_INPUT", "if (!(condition)) {  throw std::invalid_argument(message);  }", ["condition", "message"])
VALIDATE_INPUT_NO_OP_DEFINE = DefineDirective("VALIDATE_INPUT", "// no validation", ["condition", "message"])

# =============================================================================
# FRAMEWORK MACRO DEFINES
# =============================================================================
# Service injection
INJECT_SERVICE_DEFINE = DefineDirective("INJECT_SERVICE", "type* get##type() { return container.resolve<type>(); }", ["type"])
REGISTER_SERVICE_DEFINE = DefineDirective("REGISTER_SERVICE", "container.register<type, impl>();", ["type", "impl"])
SINGLETON_DEFINE = DefineDirective("SINGLETON", "static type* instance = nullptr; if (!instance) instance = new type();", ["type"])

# Feature toggles
ENABLE_FEATURE_DEFINE = DefineDirective("ENABLE_FEATURE", "constexpr bool name##_enabled = true;", ["name"])
DISABLE_FEATURE_DEFINE = DefineDirective("DISABLE_FEATURE", "constexpr bool name##_enabled = false;", ["name"])

# Async/cache features
ASYNC_METHOD_DEFINE = DefineDirective("ASYNC_METHOD", "std::future<ret> name##Async(params) {  return std::async(std::launch::async, [this](params) {  return this->name(params);  });  }", ["ret", "name", "params"])
ASYNC_METHOD_DISABLED_DEFINE = DefineDirective("ASYNC_METHOD", "// disabled", ["ret", "name", "params"])
CACHED_RESULT_DEFINE = DefineDirective("CACHED_RESULT", "static std::unordered_map<std::string, type> cache_##key;  if (cache_##key.find(#key) != cache_##key.end())  return cache_##key[#key];", ["type", "key"])
CACHED_RESULT_DISABLED_DEFINE = DefineDirective("CACHED_RESULT", "// no caching", ["type", "key"])

# =============================================================================
# GENERATED #DEFINE STRINGS - BASIC
# =============================================================================
SIMPLE_OBJECT_MACRO = to_define_string(PI_DEFINE)
SIMPLE_FUNCTION_MACRO = to_define_string(MAX_DEFINE)
EMPTY_MACRO = to_define_string(EMPTY_DEFINE)
ADD = to_define_string(ADD_DEFINE)
DIVIDE = to_define_string(DIVIDE_DEFINE)

# =============================================================================
# GENERATED #DEFINE STRINGS - TOKEN MANIPULATION
# =============================================================================
CONCAT_MACRO = to_define_string(CONCAT_DEFINE)
STRINGIFY_MACRO = to_define_string(STRINGIFY_DEFINE)
QUOTE_PARAM_MACRO = to_define_string(QUOTE_PARAM_DEFINE)
PREFIX_MACRO = to_define_string(PREFIX_DEFINE)
SUFFIX_MACRO = to_define_string(SUFFIX_DEFINE)
MIXED_STRINGIFY_CONCAT_MACRO = to_define_string(MIXED1_DEFINE)

# =============================================================================
# GENERATED #DEFINE STRINGS - MULTI-LINE
# =============================================================================
LONG_MACRO_DEFINITION = to_multiline_define_string(LONG_MACRO_DEFINE)
MAX_MACRO_DEFINITION = to_multiline_define_string(MAX_LINE_CONT_DEFINE)
COMPLEX_MACRO_DEFINITION = to_multiline_define_string(COMPLEX_DEFINE)
PROPERTY_MACRO_DEFINITION = to_multiline_define_string(PROPERTY_DEFINE)
SMART_POINTER_MACRO_DEFINITION = to_multiline_define_string(SMART_POINTER_DEFINE)

# =============================================================================
# GENERATED #DEFINE STRINGS - CONDITIONAL
# =============================================================================
DEBUG_LOG_MACRO = to_define_string(LOG_DEBUG_MSG_DEFINE)
NO_OP_LOG_MACRO = to_define_string(LOG_NO_OP_DEFINE)
VALIDATION_MACRO_DEFINITION = to_multiline_define_string(VALIDATE_INPUT_MULTILINE_DEFINE)
NO_VALIDATION_MACRO = to_define_string(VALIDATE_INPUT_NO_OP_DEFINE)

# =============================================================================
# GENERATED #DEFINE STRINGS - FRAMEWORK
# =============================================================================
INJECT_SERVICE_MACRO = to_define_string(INJECT_SERVICE_DEFINE)
REGISTER_SERVICE_MACRO = to_define_string(REGISTER_SERVICE_DEFINE)
SINGLETON_MACRO = to_define_string(SINGLETON_DEFINE)
ENABLE_FEATURE_MACRO = to_define_string(ENABLE_FEATURE_DEFINE)
DISABLE_FEATURE_MACRO = to_define_string(DISABLE_FEATURE_DEFINE)
ASYNC_METHOD_MACRO_DEFINITION = to_multiline_define_string(ASYNC_METHOD_DEFINE)
CACHED_RESULT_MACRO_DEFINITION = to_multiline_define_string(CACHED_RESULT_DEFINE)

# =============================================================================
# GENERATED #DEFINE STRINGS - NESTED
# =============================================================================
NESTED_CONCAT_MACRO = to_define_string(MAKE_GETTER_DEFINE)
NESTED_SETTER_MACRO = to_define_string(MAKE_SETTER_DEFINE)

# =============================================================================
# PREPROCESSOR FLAGS
# =============================================================================
DEBUG_MODE_FLAG = "DEBUG_MODE"
USE_VALIDATION_FLAG = "USE_VALIDATION"
FEATURE_ASYNC_FLAG = "FEATURE_ASYNC"
FEATURE_CACHE_FLAG = "FEATURE_CACHE"
DEBUG_FLAG = "DEBUG"
FEATURE_A_FLAG = "FEATURE_A"
FEATURE_B_FLAG = "FEATURE_B"

# =============================================================================
# SIMPLE TEST CASES
# =============================================================================
NO_MACRO_DEFINITION = """INVOKE(method, arg0, arg1, arg2);"""

SIMPLE_OBJECT_MACRO_CPP = f"""#define PI 3.14159
#define VERSION 42

float radius = PI * 2;
int ver = VERSION;"""

SIMPLE_FUNCTION_MACRO_CPP = f"""{SIMPLE_FUNCTION_MACRO}
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int max_val = MAX(10, 20);
int min_val = MIN(5, 3);"""

EMPTY_MACRO_CPP = f"""#define EMPTY
#define PLACEHOLDER

EMPTY int x = 5; PLACEHOLDER"""

UNDEF_MACRO_CPP = f"""#define TEMP 100
int a = TEMP;
#undef TEMP
#define TEMP 200
int b = TEMP;"""

NO_PARAMS_FUNCTION_MACRO_CPP = f"""#define INIT() {{ initialized = true; }}
#define CLEANUP() {{ cleanup_done = true; }}

void setup() INIT()
void teardown() CLEANUP()"""

MULTI_PARAM_MACRO_CPP = f"""#define FUNC(name, ret, arg1, arg2) ret name(arg1 a, arg2 b) {{ return a + b; }}

FUNC(add, int, int, int)
FUNC(concat, float, float, float)"""

STRINGIFICATION_ONLY_CPP = f"""{to_define_string(STR_DEFINE)}
{to_define_string(QUOTE_DEFINE)}

const char* name = STR(variable);
const char* msg = QUOTE(hello world);"""

TOKEN_PASTING_ONLY_CPP = f"""{CONCAT_MACRO}
{to_define_string(MAKE_VAR_DEFINE)}

int CONCAT(my, Var) = 10;
int MAKE_VAR(test, value) = 20;"""

CONDITIONAL_COMPILATION_CPP = f"""#ifdef DEBUG
#define LOG(x) printf(x)
#else
#define LOG(x)
#endif

void test() {{
    LOG("debug message");
}}"""

NESTED_CONDITIONALS_CPP = f"""#ifdef FEATURE_A
    #ifdef FEATURE_B
        #define MODE "AB"
    #else
        #define MODE "A"
    #endif
#else
    #define MODE "NONE"
#endif

const char* mode = MODE;"""

# =============================================================================
# COMPLEX TEST CASES
# =============================================================================
NESTED_MACRO_INVOCATION = f"""{SIMPLE_FUNCTION_MACRO}
{ADD}
{DIVIDE}

MAX(ADD(DIVIDE(1.0, 2.0), 3.0), DIVIDE(4.0, 5.0));"""

LINE_CONTINUATION_CPP = f"""{LONG_MACRO_DEFINITION}

{MAX_MACRO_DEFINITION}

{SIMPLE_OBJECT_MACRO}
{COMPLEX_MACRO_DEFINITION}
#define ANOTHER 24

LONG_MACRO("test");
int x = MAX(5, 10);
int y = PI + ANOTHER;
COMPLEX(PI);"""

TOKEN_PASTING_CPP = f"""{CONCAT_MACRO}
{STRINGIFY_MACRO}
{QUOTE_PARAM_MACRO}
{PREFIX_MACRO}
{SUFFIX_MACRO}
{MIXED_STRINGIFY_CONCAT_MACRO}

CONCAT(hello, world);
STRINGIFY(test);
QUOTE_PARAM(param);
PREFIX(function);
SUFFIX(variable);
MIXED1(test);"""

DEPENDENCY_INJECTION_CPP = f"""{INJECT_SERVICE_MACRO}
{REGISTER_SERVICE_MACRO}
{SINGLETON_MACRO}

#ifdef DEBUG_MODE
{DEBUG_LOG_MACRO}
#else
{NO_OP_LOG_MACRO}
#endif

class ServiceContainer {{
public:
    INJECT_SERVICE(DatabaseService)
    INJECT_SERVICE(LoggingService)
    
    void initialize() {{
        LOG("Initializing container")
        REGISTER_SERVICE(DatabaseService, MySQLDatabase)
        REGISTER_SERVICE(LoggingService, FileLogger)
    }}
    
    void cleanup() {{
        LOG("Cleaning up container")
    }}
}};

class Application {{
private:
    SINGLETON(Application)
    
public:
    void run() {{
        LOG("Application starting")
        auto db = getDatabaseService();
        auto logger = getLoggingService();
    }}
}};"""

TEMPLATE_METAPROGRAMMING_CPP = f"""{ENABLE_FEATURE_MACRO}
{DISABLE_FEATURE_MACRO}

#ifdef FEATURE_ASYNC
ENABLE_FEATURE(async)
{ASYNC_METHOD_MACRO_DEFINITION}
#else
DISABLE_FEATURE(async)
#define ASYNC_METHOD(ret, name, params) // disabled
#endif

#ifdef FEATURE_CACHE
ENABLE_FEATURE(cache)
{CACHED_RESULT_MACRO_DEFINITION}
#else
DISABLE_FEATURE(cache)
#define CACHED_RESULT(type, key) // no caching
#endif

template<typename T>
class DataProcessor {{
public:
    T processData(const T& input) {{
        CACHED_RESULT(T, processData)
        
        T result = complexOperation(input);
        
        #ifdef FEATURE_CACHE
        cache_processData["processData"] = result;
        #endif
        
        return result;
    }}
    
    ASYNC_METHOD(T, processData, const T& input)
    
private:
    T complexOperation(const T& input) {{
        return input * 2;
    }}
}};"""

MULTI_LEVEL_MACRO_CPP = f"""{CONCAT_MACRO}
{NESTED_CONCAT_MACRO}
{NESTED_SETTER_MACRO}

{PROPERTY_MACRO_DEFINITION}

{SMART_POINTER_MACRO_DEFINITION}

#ifdef USE_VALIDATION
{VALIDATION_MACRO_DEFINITION}
#else
{NO_VALIDATION_MACRO}
#endif

class ConfigManager {{
    PROPERTY(std::string, ConfigPath)
    PROPERTY(int, MaxConnections)
    PROPERTY(bool, EnableLogging)
    
    SMART_POINTER(Database)
    SMART_POINTER(Logger)
    
public:
    void setConfiguration(const std::string& path, int connections) {{
        VALIDATE_INPUT(!path.empty(), "Config path cannot be empty")
        VALIDATE_INPUT(connections > 0, "Max connections must be positive")
        
        setConfigPath(path);
        setMaxConnections(connections);
    }}
    
    void initializeServices() {{
        DatabasePtr = std::make_shared<Database>();
        LoggerPtr = std::make_shared<Logger>();
    }}
}};

#undef VALIDATE_INPUT
#define VALIDATE_INPUT(condition, message) assert(condition && message);"""

# =============================================================================
# EXPECTED OUTPUTS - SIMPLE
# =============================================================================
EXPECTED_NO_MACRO_DEFINITION = f"""{NO_MACRO_DEFINITION}"""
EXPECTED_EMPTY_MACRO = """int x = 5;"""
EXPECTED_SIMPLE_OBJECT_MACRO = """float radius = 3.14159 * 2;
int ver = 42;"""
EXPECTED_SIMPLE_FUNCTION_MACRO = """int max_val = ((10) > (20) ? (10) : (20));
int min_val = ((5) < (3) ? (5) : (3));"""
EXPECTED_NO_PARAMS_FUNCTION_MACRO = """void setup() { initialized = true; }
void teardown() { cleanup_done = true; }"""
EXPECTED_MULTI_PARAM_MACRO = """int add(int a, int b) { return a + b; }
float concat(float a, float b) { return a + b; }"""
EXPECTED_UNDEF_MACRO = """int a = 100;

int b = 200;"""
EXPECTED_STRINGIFICATION_ONLY = """const char* name = "variable";
const char* msg = "hello world";"""
EXPECTED_TOKEN_PASTING_ONLY = """int myVar = 10;
int test_value = 20;"""
EXPECTED_CONDITIONAL_DEBUG = """void test() {
    printf("debug message");
}"""
EXPECTED_CONDITIONAL_NO_DEBUG = """void test() {
    ;
}"""
EXPECTED_NESTED_AB = """const char* mode = "AB";"""
EXPECTED_NESTED_A_ONLY = """const char* mode = "A";"""
EXPECTED_NESTED_NONE = """const char* mode = "NONE";"""

# =============================================================================
# EXPECTED OUTPUTS - COMPLEX
# =============================================================================
EXPECTED_NESTED_INVOCATION = (
    "((((1.0 / 2.0) + 3.0)) > ((4.0 / 5.0)) ? "
    "(((1.0 / 2.0) + 3.0)) : ((4.0 / 5.0)));"
)
EXPECTED_LINE_CONTINUATION_OUTPUT = (
    "do {  printf(\"%s\", \"test\");  } while(0);\n"
    "int x = ((5) > (10) ?  (5) : (10));\n"
    "int y = 3.14159 + 24;\n"
    "if (3.14159 > 0) {  return 3.14159 * 2;  };"
)
EXPECTED_TOKEN_PASTING_OUTPUT = (
    "helloworld;\n"
    "\"test\";\n"
    "\"x\";\n"
    "prefix_function;\n"
    "variable_suffix;\n"
    "prefix_\"test\"_suffix;"
)
EXPECTED_DEPENDENCY_INJECTION_DEBUG = """class ServiceContainer {
public:
    DatabaseService* getDatabaseService() { return container.resolve<DatabaseService>(); }
    LoggingService* getLoggingService() { return container.resolve<LoggingService>(); }
    
    void initialize() {
        std::cout << "[DEBUG] " << "Initializing container" << std::endl;
        container.register<DatabaseService, MySQLDatabase>();
        container.register<LoggingService, FileLogger>();
    }
    
    void cleanup() {
        std::cout << "[DEBUG] " << "Cleaning up container" << std::endl;
    }
};

class Application {
private:
    static Application* instance = nullptr; if (!instance) instance = new Application();
    
public:
    void run() {
        std::cout << "[DEBUG] " << "Application starting" << std::endl;
        auto db = getDatabaseService();
        auto logger = getLoggingService();
    }
};"""
EXPECTED_DEPENDENCY_INJECTION_NO_DEBUG = """class ServiceContainer {
public:
    DatabaseService* getDatabaseService() { return container.resolve<DatabaseService>(); }
    LoggingService* getLoggingService() { return container.resolve<LoggingService>(); }
    
    void initialize() {
        // no-op
        container.register<DatabaseService, MySQLDatabase>();
        container.register<LoggingService, FileLogger>();
    }
    
    void cleanup() {
        // no-op
    }
};

class Application {
private:
    static Application* instance = nullptr; if (!instance) instance = new Application();
    
public:
    void run() {
        // no-op
        auto db = getDatabaseService();
        auto logger = getLoggingService();
    }
};"""
EXPECTED_TEMPLATE_METAPROGRAMMING_ENABLED = (
    "constexpr bool async_enabled = true;\n"
    "\n"
    "\n"
    "constexpr bool cache_enabled = true;\n"
    "\n"
    "\n"
    "template<typename T>\n"
    "class DataProcessor {\n"
    "public:\n"
    "    T processData(const T& input) {\n"
    "        static std::unordered_map<std::string, T> cache_processData;  "
    "if (cache_processData.find(\"processData\") != cache_processData.end())  "
    "return cache_processData[\"processData\"];\n"
    "        \n"
    "        T result = complexOperation(input);\n"
    "        \n"
    "        cache_processData[\"processData\"] = result;\n"
    "        \n"
    "        return result;\n"
    "    }\n"
    "    \n"
    "    std::future<T> processDataAsync(const T& input) {  "
    "return std::async(std::launch::async, [this](const T& input) {  "
    "return this->processData(const T& input);  });  }\n"
    "    \n"
    "private:\n"
    "    T complexOperation(const T& input) {\n"
    "        return input * 2;\n"
    "    }\n"
    "};"
)
EXPECTED_TEMPLATE_METAPROGRAMMING_DISABLED = (
    "constexpr bool async_enabled = false;\n"
    "\n"
    "\n"
    "constexpr bool cache_enabled = false;\n"
    "\n"
    "\n"
    "template<typename T>\n"
    "class DataProcessor {\n"
    "public:\n"
    "    T processData(const T& input) {\n"
    "        // no caching\n"
    "        \n"
    "        T result = complexOperation(input);\n"
    "        \n"
    "        \n"
    "        return result;\n"
    "    }\n"
    "    \n"
    "    // disabled\n"
    "    \n"
    "private:\n"
    "    T complexOperation(const T& input) {\n"
    "        return input * 2;\n"
    "    }\n"
    "};"
)
EXPECTED_MULTI_LEVEL_MACRO_VALIDATION = (
    "class ConfigManager {\n"
    "    private:  std::string m_ConfigPath;  public:  std::string getConfigPath() const { return m_ConfigPath; }  "
    "void setConfigPath(const std::string& value) { m_ConfigPath = value; }\n"
    "    private:  int m_MaxConnections;  public:  int getMaxConnections() const { return m_MaxConnections; }  "
    "void setMaxConnections(const int& value) { m_MaxConnections = value; }\n"
    "    private:  bool m_EnableLogging;  public:  bool getEnableLogging() const { return m_EnableLogging; }  "
    "void setEnableLogging(const bool& value) { m_EnableLogging = value; }\n"
    "    \n"
    "    std::shared_ptr<Database> DatabasePtr;  std::weak_ptr<Database> DatabaseWeakPtr;  "
    "std::unique_ptr<Database> DatabaseUniquePtr;\n"
    "    std::shared_ptr<Logger> LoggerPtr;  std::weak_ptr<Logger> LoggerWeakPtr;  "
    "std::unique_ptr<Logger> LoggerUniquePtr;\n"
    "    \n"
    "public:\n"
    "    void setConfiguration(const std::string& path, int connections) {\n"
    "        if (!(!path.empty())) {  throw std::invalid_argument(\"Config path cannot be empty\");  }\n"
    "        if (!(connections > 0)) {  throw std::invalid_argument(\"Max connections must be positive\");  }\n"
    "        \n"
    "        setConfigPath(path);\n"
    "        setMaxConnections(connections);\n"
    "    }\n"
    "    \n"
    "    void initializeServices() {\n"
    "        DatabasePtr = std::make_shared<Database>();\n"
    "        LoggerPtr = std::make_shared<Logger>();\n"
    "    }\n"
    "};"
)
EXPECTED_MULTI_LEVEL_MACRO_NO_VALIDATION = (
    "class ConfigManager {\n"
    "    private:  std::string m_ConfigPath;  public:  std::string getConfigPath() const { return m_ConfigPath; }  "
    "void setConfigPath(const std::string& value) { m_ConfigPath = value; }\n"
    "    private:  int m_MaxConnections;  public:  int getMaxConnections() const { return m_MaxConnections; }  "
    "void setMaxConnections(const int& value) { m_MaxConnections = value; }\n"
    "    private:  bool m_EnableLogging;  public:  bool getEnableLogging() const { return m_EnableLogging; }  "
    "void setEnableLogging(const bool& value) { m_EnableLogging = value; }\n"
    "    \n"
    "    std::shared_ptr<Database> DatabasePtr;  std::weak_ptr<Database> DatabaseWeakPtr;  "
    "std::unique_ptr<Database> DatabaseUniquePtr;\n"
    "    std::shared_ptr<Logger> LoggerPtr;  std::weak_ptr<Logger> LoggerWeakPtr;  "
    "std::unique_ptr<Logger> LoggerUniquePtr;\n"
    "    \n"
    "public:\n"
    "    void setConfiguration(const std::string& path, int connections) {\n"
    "        // no validation\n"
    "        // no validation\n"
    "        \n"
    "        setConfigPath(path);\n"
    "        setMaxConnections(connections);\n"
    "    }\n"
    "    \n"
    "    void initializeServices() {\n"
    "        DatabasePtr = std::make_shared<Database>();\n"
    "        LoggerPtr = std::make_shared<Logger>();\n"
    "    }\n"
    "};"
)

# =============================================================================
# INCLUDE TEST CASES
# =============================================================================

# Header file macros
HEADER_MACRO_DEFINE = DefineDirective("HEADER_MACRO", "42")

HEADER_CONTENT = (
    "#ifndef HEADER_HPP\n"
    "#define HEADER_HPP\n"
    "#define HEADER_MACRO 42\n"
    "int header_var = HEADER_MACRO;\n"
    "#endif // HEADER_HPP"
)

INCLUDE_AT_BEGINNING_CPP = (
    "#include \"header.hpp\"\n"
    "int main_var = HEADER_MACRO + 1;"
)

EXPECTED_INCLUDE_AT_BEGINNING = "int main_var = 42 + 1;"

UTIL_CONTENT = (
    "#ifndef UTIL_HPP\n"
    "#define UTIL_HPP\n"
    "inline int add(int a, int b) { return a + b; }\n"
    "#endif // UTIL_HPP"
)

INCLUDE_INSIDE_CODE_CPP = (
    "int x = 10;\n"
    "#include \"util.hpp\"\n"
    "int result = add(x, 5);"
)

EXPECTED_INCLUDE_INSIDE_CODE = (
    "int x = 10;\n"
    "inline int add(int a, int b) { return a + b; }\n"
    "int result = add(x, 5);"
)

# =============================================================================
# COMPREHENSIVE NESTED INCLUDE TEST CONSTANTS
# =============================================================================

# Leaf level headers (level 3)
LEAF1_CONTENT = (
    "#ifndef LEAF1_HPP\n"
    "#define LEAF1_HPP\n"
    "#define LEAF1_MACRO 100\n"
    "int leaf1_var = LEAF1_MACRO;\n"
    "#endif // LEAF1_HPP"
)
LEAF2_CONTENT = (
    "#ifndef LEAF2_HPP\n"
    "#define LEAF2_HPP\n"
    "#define LEAF2_MACRO 200\n"
    "int leaf2_var = LEAF2_MACRO;\n"
    "#endif // LEAF2_HPP"
)
LEAF3_CONTENT = (
    "#ifndef LEAF3_HPP\n"
    "#define LEAF3_HPP\n"
    "#define LEAF3_MACRO 300\n"
    "int leaf3_var = LEAF3_MACRO;\n"
    "#endif // LEAF3_HPP"
)
LEAF4_CONTENT = (
    "#ifndef LEAF4_HPP\n"
    "#define LEAF4_HPP\n"
    "#define LEAF4_MACRO 400\n"
    "int leaf4_var = LEAF4_MACRO;\n"
    "#endif // LEAF4_HPP"
)
LEAF5_CONTENT = (
    "#ifndef LEAF5_HPP\n"
    "#define LEAF5_HPP\n"
    "#define LEAF5_MACRO 500\n"
    "int leaf5_var = LEAF5_MACRO;\n"
    "#endif // LEAF5_HPP"
)

# Mid level headers (level 2)
MID1_CONTENT = (
    "#ifndef MID1_HPP\n"
    "#define MID1_HPP\n"
    "#include \"leaf/leaf1.hpp\"\n"
    "#include \"leaf/leaf2.hpp\"\n"
    "#define MID1_MACRO 10\n"
    "int mid1_var = MID1_MACRO + LEAF1_MACRO;\n"
    "#endif // MID1_HPP"
)

MID2_CONTENT = (
    "#ifndef MID2_HPP\n"
    "#define MID2_HPP\n"
    "#include \"leaf/leaf3.hpp\"\n"
    "#define MID2_MACRO 20\n"
    "int mid2_var = MID2_MACRO + LEAF3_MACRO;\n"
    "#endif // MID2_HPP"
)

MID3_CONTENT = (
    "#ifndef MID3_HPP\n"
    "#define MID3_HPP\n"
    "#include \"leaf/leaf4.hpp\"\n"
    "#include \"leaf/leaf5.hpp\"\n"
    "#include \"leaf/leaf1.hpp\"\n"
    "#define MID3_MACRO 30\n"
    "int mid3_var = MID3_MACRO + LEAF4_MACRO;\n"
    "#endif // MID3_HPP"
)

# Top level header (level 1)
TOP_CONTENT = (
    "#ifndef TOP_HPP\n"
    "#define TOP_HPP\n"
    "#include \"mid/mid1.hpp\"\n"
    "#include \"mid/mid2.hpp\"\n"
    "#include \"mid/mid3.hpp\"\n"
    "#define TOP_MACRO 1\n"
    "int top_var = TOP_MACRO + MID1_MACRO;\n"
    "#endif // TOP_HPP"
)

# Main file
MAIN_NESTED_CONTENT = (
    "#include \"top/top.hpp\"\n"
    "int main() {\n"
    "    return TOP_MACRO + MID1_MACRO + LEAF1_MACRO;\n"
    "}"
)

# Define directives for nested includes
LEAF1_MACRO_DEFINE = DefineDirective("LEAF1_MACRO", "100")
LEAF2_MACRO_DEFINE = DefineDirective("LEAF2_MACRO", "200")
LEAF3_MACRO_DEFINE = DefineDirective("LEAF3_MACRO", "300")
LEAF4_MACRO_DEFINE = DefineDirective("LEAF4_MACRO", "400")
LEAF5_MACRO_DEFINE = DefineDirective("LEAF5_MACRO", "500")
MID1_MACRO_DEFINE = DefineDirective("MID1_MACRO", "10")
MID2_MACRO_DEFINE = DefineDirective("MID2_MACRO", "20")
MID3_MACRO_DEFINE = DefineDirective("MID3_MACRO", "30")
TOP_MACRO_DEFINE = DefineDirective("TOP_MACRO", "1")

# Expected processed content
EXPECTED_LEAF1_PROCESSED = "int leaf1_var = 100;"
EXPECTED_LEAF2_PROCESSED = "int leaf2_var = 200;"
EXPECTED_LEAF3_PROCESSED = "int leaf3_var = 300;"
EXPECTED_LEAF4_PROCESSED = "int leaf4_var = 400;"
EXPECTED_LEAF5_PROCESSED = "int leaf5_var = 500;"
EXPECTED_MID1_PROCESSED = "int mid1_var = 10 + 100;"
EXPECTED_MID2_PROCESSED = "int mid2_var = 20 + 300;"
EXPECTED_MID3_PROCESSED = "int mid3_var = 30 + 400;"
EXPECTED_TOP_PROCESSED = "int top_var = 1 + 10;"
EXPECTED_MAIN_NESTED_PROCESSED = (
    "int main() {\n"
    "    return 1 + 10 + 100;\n"
    "}"
)

# =============================================================================
# METAPROGRAMMING INLINE INCLUDE TEST CONSTANTS
# =============================================================================

# Template snippets for metaprogramming
TEMPLATE_GETTER_CONTENT = (
    "#ifndef GETTER_HPP\n"
    "#define GETTER_HPP\n"
    "T get##name() const { return m_##name; }\n"
    "#endif // GETTER_HPP"
)
TEMPLATE_SETTER_CONTENT = (
    "#ifndef SETTER_HPP\n"
    "#define SETTER_HPP\n"
    "void set##name(const T& value) { m_##name = value; }\n"
    "#endif // SETTER_HPP"
)
TEMPLATE_MEMBER_CONTENT = (
    "#ifndef MEMBER_HPP\n"
    "#define MEMBER_HPP\n"
    "T m_##name;\n"
    "#endif // MEMBER_HPP"
)

# Class template that uses inline includes for metaprogramming
METAPROGRAMMED_CLASS_CONTENT = (
    "template<typename T>\n"
    "class Property {\n"
    "private:\n"
    "    #include \"templates/member.hpp\"\n"
    "public:\n"
    "    #include \"templates/getter.hpp\"\n"
    "    #include \"templates/setter.hpp\"\n"
    "};\n"
    "\n"
    "Property<int> intProperty;"
)

# Expected result with inlined template content
EXPECTED_METAPROGRAMMED_PROCESSED = (
    "template<typename T>\n"
    "class Property {\n"
    "private:\n"
    "    T m_##name;\n"
    "public:\n"
    "    T get##name() const { return m_##name; }\n"
    "    void set##name(const T& value) { m_##name = value; }\n"
    "};\n"
    "\n"
    "Property<int> intProperty;"
)

# =============================================================================
# DEPENDENT METAPROGRAMMING TEST CONSTANTS
# =============================================================================

# Parent class that defines a macro used by inlined templates
PARENT_CLASS_CONTENT = (
    "#ifndef PARENT_HPP\n"
    "#define PARENT_HPP\n"
    "#define FIELD_TYPE int\n"
    "template<typename T>\n"
    "class Container {\n"
    "private:\n"
    "    #include \"templates/field.hpp\"\n"
    "public:\n"
    "    #include \"templates/accessor.hpp\"\n"
    "};\n"
    "#endif // PARENT_HPP"
)

# Template that depends on FIELD_TYPE macro from parent
TEMPLATE_FIELD_CONTENT = (
    "#ifndef FIELD_HPP\n"
    "#define FIELD_HPP\n"
    "FIELD_TYPE m_data;\n"
    "#endif // FIELD_HPP"
)

# Template accessor that also uses FIELD_TYPE
TEMPLATE_ACCESSOR_CONTENT = (
    "#ifndef ACCESSOR_HPP\n"
    "#define ACCESSOR_HPP\n"
    "FIELD_TYPE getData() const { return m_data; }\n"
    "void setData(const FIELD_TYPE& value) { m_data = value; }\n"
    "#endif // ACCESSOR_HPP"
)

# Main file that uses the parent class
DEPENDENT_METAPROGRAMMING_CONTENT = (
    "#include \"parent.hpp\"\n"
    "Container<double> container;"
)

# Expected processed content with macro expansion
EXPECTED_DEPENDENT_METAPROGRAMMING_PROCESSED = (
    "Container<double> container;"
)

# Expected parent class content after processing
EXPECTED_PARENT_CLASS_PROCESSED = (
    "template<typename T>\n"
    "class Container {\n"
    "private:\n"
    "    int m_data;\n"
    "public:\n"
    "    int getData() const { return m_data; }\n"
    "    void setData(const int& value) { m_data = value; }\n"
    "};"
)

# Header guard defines for dependent test
PARENT_HPP_DEFINE = DefineDirective("PARENT_HPP", "")
FIELD_HPP_DEFINE = DefineDirective("FIELD_HPP", "")
ACCESSOR_HPP_DEFINE = DefineDirective("ACCESSOR_HPP", "")
FIELD_TYPE_DEFINE = DefineDirective("FIELD_TYPE", "int")

# =============================================================================
# COMMONLY USED DEFINE DIRECTIVES FOR TESTS
# =============================================================================
FEATURE_A_DEFINE = DefineDirective(FEATURE_A_FLAG, "1")
FEATURE_B_DEFINE = DefineDirective(FEATURE_B_FLAG, "1")
DEBUG_MODE_DEFINE = DefineDirective(DEBUG_MODE_FLAG, "1")
USE_VALIDATION_DEFINE = DefineDirective(USE_VALIDATION_FLAG, "1")
FEATURE_ASYNC_DEFINE = DefineDirective(FEATURE_ASYNC_FLAG, "1")
FEATURE_CACHE_DEFINE = DefineDirective(FEATURE_CACHE_FLAG, "1")

# =============================================================================
# EXPECTED PROCESSED CONTENT CONSTANTS
# =============================================================================
EXPECTED_HEADER_PROCESSED = "int header_var = 42;"

# ParameterizedFile objects for include tests
from src.preprocess.constructs.parameterized_file import ParameterizedFile

# Header guard macro definitions
HEADER_HPP_DEFINE = DefineDirective("HEADER_HPP", "")
UTIL_HPP_DEFINE = DefineDirective("UTIL_HPP", "")
LEAF1_HPP_DEFINE = DefineDirective("LEAF1_HPP", "")
LEAF2_HPP_DEFINE = DefineDirective("LEAF2_HPP", "")
LEAF3_HPP_DEFINE = DefineDirective("LEAF3_HPP", "")
LEAF4_HPP_DEFINE = DefineDirective("LEAF4_HPP", "")
LEAF5_HPP_DEFINE = DefineDirective("LEAF5_HPP", "")
MID1_HPP_DEFINE = DefineDirective("MID1_HPP", "")
MID2_HPP_DEFINE = DefineDirective("MID2_HPP", "")
MID3_HPP_DEFINE = DefineDirective("MID3_HPP", "")
TOP_HPP_DEFINE = DefineDirective("TOP_HPP", "")
GETTER_HPP_DEFINE = DefineDirective("GETTER_HPP", "")
SETTER_HPP_DEFINE = DefineDirective("SETTER_HPP", "")
MEMBER_HPP_DEFINE = DefineDirective("MEMBER_HPP", "")

HEADER_PARAMETERIZED_FILE = ParameterizedFile(
    file_path="header.hpp",
    raw_content=EXPECTED_HEADER_PROCESSED,
    defined_macros={"HEADER_MACRO": HEADER_MACRO_DEFINE, "HEADER_HPP": HEADER_HPP_DEFINE},
    included_files=[]
)

UTIL_PARAMETERIZED_FILE = ParameterizedFile(
    file_path="util.hpp",
    raw_content="inline int add(int a, int b) { return a + b; }",
    defined_macros={"UTIL_HPP": UTIL_HPP_DEFINE},
    included_files=[]
)

MAIN_WITH_HEADER_PARAMETERIZED_FILE = ParameterizedFile(
    file_path="main.cpp",
    raw_content=EXPECTED_INCLUDE_AT_BEGINNING,
    defined_macros={"HEADER_MACRO": HEADER_MACRO_DEFINE, "HEADER_HPP": HEADER_HPP_DEFINE},
    included_files=[HEADER_PARAMETERIZED_FILE]
)

MAIN_WITH_UTIL_PARAMETERIZED_FILE = ParameterizedFile(
    file_path="main.cpp",
    raw_content=EXPECTED_INCLUDE_INSIDE_CODE,
    defined_macros={},
    included_files=[UTIL_PARAMETERIZED_FILE]
)