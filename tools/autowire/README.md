Autowire is a C++ dependency injection generator that automatically injects dependencies into objects, while autowiring unknown dependencies to the command-line

# Architecture

## Entry Point

Define entry points using class and method annotations:

**AUTOWIRE on classes** - marks classes as entry points:

*Standalone entry point (no subcommands):*
```cpp
/**
 * Simple tool with direct entry point
 * AUTOWIRE
 */
class SimpleTool {
public:
    SimpleTool(const std::string& inputFile, double threshold);
    void run();
};
```

*Subcommand system (base + subclasses):*
```cpp
/**
 * Base algorithm interface for subcommand system
 * AUTOWIRE
 */
class Algorithm {
public:
    virtual void run() = 0;
};

/**
 * Distance algorithm for satellite positioning
 * AUTOWIRE(distance)
 */
class DistanceAlgorithm : public Algorithm {
public:
    DistanceAlgorithm(const std::string& imagePath, double focalLength);
    void run() override;
};
```

**Two modes:**
- `AUTOWIRE` alone = standalone entry point, generates `createSimpleTool(argc, argv)`
- `AUTOWIRE` + `AUTOWIRE(subcommand)` = subcommand system with routing

**Inheritance requirement for subcommands:**
- Classes with `AUTOWIRE(subcommand)` MUST inherit from a class with `AUTOWIRE`
- Tool validates inheritance hierarchy and rejects invalid configurations

**PROVIDER on methods** - marks methods as dependency providers:
```cpp
/**
 * Provides camera configuration parameters
 * PROVIDER
 */
CameraParams provideCameraParams(double focalLength, double pixelSize);
```

Optional:
- Root (folder containing the src folder)
- If provider is in a class, the class must be no-args instantiable

## Parsing Strategy

**Recursive Include-Driven Parsing:**
1. **Start with Target Files** - begin with files containing AUTOWIRE/PROVIDER annotations
2. **Parse Includes First** - recursively parse all `#include` statements to build complete symbol context
3. **Build Global Context** - accumulate macro_table, alias_table, type_definitions across all included files
4. **Parse Entry Points** - find AUTOWIRE classes and their constructor dependencies
5. **Recursive Dependency Resolution** - for each constructor parameter type, recursively parse its includes and dependencies

**Include Resolution Process:**
1. **Parse Include Statements** - extract `#include "file.h"` and `#include <system.h>`
2. **Resolve Include Paths** - find actual file paths using include search paths
3. **Recursive Parse** - parse included files first, building up symbol tables
4. **Merge Contexts** - combine macro_table, alias_table, type_definitions from included files
5. **Parse Current File** - parse current file with complete context from all includes

**Dependency Tree Construction (Leaves-Up):**
1. **Parse Leaf Dependencies First** - start with deepest includes (system headers, base classes)
2. **Build Symbol Tables** - accumulate type definitions from leaf files up through include hierarchy
3. **Parse Intermediate Dependencies** - files that depend on leaf files but are dependencies of others
4. **Parse Entry Points Last** - AUTOWIRE classes at the top, with complete context of all dependencies
5. **Bottom-Up Resolution** - each level can resolve types because lower levels are already parsed

**Example Parsing Order:**
```
<vector> (system header)     ← Parse first (leaf)
    ↓
BaseClass.h                  ← Parse second  
    ↓
MyClass.h                    ← Parse third
    ↓
main.cpp (AUTOWIRE)          ← Parse last (root)
```

**Validation:**
1. All `AUTOWIRE(subcommand)` classes must inherit from an `AUTOWIRE` base class
2. All include dependencies are resolvable (no missing header files)
3. All terminal types are either user-defined classes or basic types
4. No circular dependencies in include chain or dependency graph
5. All leaf nodes are injectable via command-line or PROVIDER functions
6. **Syntax Validation** - throw errors for invalid C++ syntax (malformed classes, functions, missing semicolons, unmatched braces, etc.)

## Dependency Injection

At this point, we have a graph relating the things we need to provide to the things that can provide it:
1. Turn the tree upside down and create methods based on that.
2. All leaf nodes should be command-line args. They should have information about their context so we know what method they came from
    - Product: An X Macro entry within the given subcommand, setup of each subcommand's options, and the generation of the parser function for the command line for that subcommand
3. All non-terminal non-leafs get provider methods based on their constructors
    a. If the node has multiple implementation classes (super node), then make sure the provider uses if statements and exposes another option about the algorithm
    - Product: Provider methods (.hpp files)
4. The terminal nodes will implement the given factories
    - Product: .hpp/.cpp files that create the autowired classes

## Write the methods

Methods should be written to an generated-src folder with all expected paths:
- The leaf node products should be written into a parser folder
- The non-terminal non-leafs should be written into the same folder as the classes they provide
- The terminal nodes should be written to the same folder as the *Create file

# Implementation

## Entry Point

The tool is a Python command-line program with minimal arguments:

```bash
python autowire.py [--root <path>] [-D<MACRO>[=<VALUE>]] ...
```

- `--root`: Optional path to project root (defaults to current directory)
- `-D<MACRO>`: Define macro for conditional compilation (e.g., `-DDEBUG`, `-DVERSION=1.2`)
- `-D<MACRO>=<VALUE>`: Define macro with value (e.g., `-DMAX_SIZE=100`, `-DPLATFORM=linux`)

## File Discovery

**FileDiscovery class:**
- `__init__(root_path: str)` - initialize with project root path
- `get_annotated_files() -> ProjectFileCache` - scan for C++ files and build project cache

**AnnotationScanner class:**
- `__init__(filepaths: List[str])` - initialize with file paths to scan
- `scan() -> Dict[str, tuple[str, bool, bool]]` - scan files for annotations using grep or file reading
- `_grep_scan()` - fast scanning using system grep command
- `_slow_scan()` - fallback file-by-file scanning when grep unavailable
- `_grep_for_annotation(pattern)` - run grep for specific annotation pattern
- `_scan_file(filepath)` - scan individual file for annotations

**Annotation Detection Strategy:**
1. **Grep-based Detection** - uses system `grep` command with regex patterns for fast scanning:
   ```bash
   grep -rlE --include=*.hpp --include=*.cpp "AUTOWIRE_PATTERN" [paths]
   ```
2. **File-by-file Fallback** - when grep unavailable, reads each C++ file individually
3. **Regex Patterns** - flexible patterns supporting whitespace and parameters:
   ```python
   AUTOWIRE_PATTERN = r"\s*\[\[\s*AUTOWIRE\s*(\([^)]*\))?\s*\]\]\s*"
   PROVIDER_PATTERN = r"\s*\[\[\s*PROVIDER\s*(\([^)]*\))?\s*\]\]\s*"
   ```

**FileInfo dataclass:**
- `file_path: str` - absolute path to source file (validated)
- `file_content: str` - complete file content
- `has_autowire: bool` - true if file contains [[AUTOWIRE]] annotations
- `has_provider: bool` - true if file contains [[PROVIDER]] annotations

**ProjectFileCache class:**
- `autowire_files: List[str]` - sorted list of files with AUTOWIRE annotations
- `provider_files: List[str]` - sorted list of files with PROVIDER annotations
- `get_all_files() -> Dict[str, FileInfo]` - all discovered files
- `get_file_content(file_path) -> str` - access file content by path
- `__contains__(file_path) -> bool` - check if file exists in cache
- `add_file(file_info: FileInfo)` - add file to cache with duplicate prevention

**Implementation Details:**
- **Dual Strategy**: Fast grep scanning with file reading fallback
- **Content Handling**: Grep mode returns None content, triggers file read when needed
- **Path Validation**: FileInfo enforces absolute paths only
- **Sorted Lists**: Consistent ordering for reliable equality comparison
- **C++ File Filtering**: Scans only .cpp and .hpp files

**Generated output**:

*For standalone AUTOWIRE:*
- `createSimpleTool(int argc, char* argv[])` - direct factory with argument parsing
- Usage: `./program --input file.txt --threshold 0.5`

*For subcommand system:*
- `createAlgorithm(int argc, char* argv[])` - unified factory with subcommand routing
- Individual subcommand parsers: `createDistanceAlgorithm()`, `createCalibrationAlgorithm()`
- Usage: `./program distance --image file.png` vs `./program calibration --ref-orientation 20,0,0`

No additional headers required - works with existing documentation and doesn't affect compilation.

## C++ Preprocessing Architecture

**Preprocessor Implementation:**
- **Preprocessor class** - handles macro expansion, conditional compilation, and include resolution
- **ParameterizedFile class** - represents preprocessed files with macro context and dependencies
- **ParameterizedFileCache class** - caches preprocessed files with different macro contexts
- **DefineDirective class** - represents #define macros with expansion capabilities
- **ConditionalDirective class** - handles #if/#ifdef/#ifndef conditional compilation
- **IncludeDirective class** - represents #include dependencies
- **MacroExpression class** - evaluates preprocessor expressions for conditionals

**Preprocessing Features:**
- **Macro Expansion** - supports object-like and function-like macros with parameters
- **Token Manipulation** - stringification (#) and token pasting (##) operators
- **Conditional Compilation** - full support for #if, #ifdef, #ifndef, #else, #elif, #endif
- **Include Processing** - handles both dependency tracking and selective inline expansion
- **Line Continuation** - processes backslash line continuations
- **Macro Undefinition** - supports #undef directive
- **Circular Include Detection** - prevents infinite recursion in include chains

**Advanced Include Handling:**
- **Beginning Includes** - tracked as dependencies via IncludeDirective objects
- **Inline Includes** - content inlined directly with preserved indentation (only for includes inside code blocks)
- **Multi-Directory Support** - recursive include path discovery
- **External Header Recognition** - distinguishes between local and system headers (`<>` vs `""`)
- **System Header Ignoring** - external headers (`#include <system.h>`) are ignored and logged
- **Metaprogramming Support** - enables template code generation via inline includes

**Preprocessor Directive Handling:**
- **Supported Directives** - #define, #undef, #if, #ifdef, #ifndef, #else, #elif, #endif, #include
- **Ignored Directives** - #pragma and other non-standard directives are ignored with logging
- **Selective Processing** - only processes directives that affect code structure and macro expansion

**Macro Context Management:**
- **Context-Aware Caching** - same file cached with different macro contexts
- **Macro Reduction** - removes unused macros for optimization
- **Recursive Expansion** - handles nested macro invocations
- **Expression Evaluation** - evaluates complex preprocessor expressions
- **Command-Line Macros** - supports -D flag macro definitions

**ParameterizedFile Structure:**
- `file_path: str` - absolute path to source file
- `raw_content: str` - preprocessed content with macros expanded
- `defined_macros: Dict[str, DefineDirective]` - macros defined in this file
- `included_files: List[ParameterizedFile]` - dependency files (beginning includes only)

**Preprocessing Pipeline:**
1. **File Discovery** - scan project for C++ files and build ProjectFileCache
2. **Include Resolution** - recursively discover all include paths
3. **Macro Processing** - expand macros and evaluate conditional compilation
4. **Dependency Tracking** - build include dependency graph
5. **Content Generation** - produce clean, expanded C++ code for parsing

## C++ Parser Architecture

**C++ Construct Objects:**
- **Class** - class and struct definitions with inheritance, constructors, methods, access specifiers
- **Function** - standalone functions and class methods with template parameters
- **Constructor** - class constructors with parameter lists
- **Destructor** - class destructors (parsed but not used for dependency injection)
- **Parameter** - function/constructor parameters with type, name, and default values
- **Enum** - enum and enum class definitions with values
- **Variable** - member variables and global variables
- **Type** - fully qualified type information with template args, qualifiers, and source construct reference
- **Value** - literal values, expressions, initializers (strings, numbers, function calls, initializer lists)
- **Statement** - abstract base class for polymorphism, defines `to_cpp()` interface
- **SimpleStatement** - single executable code line stored as raw C++ string (no parsing or semantic analysis)
- **IfStatement** - conditional execution, inherits from Statement
- **ForLoop** - for loop construct, inherits from Statement
- **WhileLoop** - while loop construct, inherits from Statement
- **SwitchStatement** - switch statement, inherits from Statement
- **Namespace** - namespace declarations and usage
- **Typedef** - type aliases and definitions
- **Using** - using declarations and using directives
- **Include** - preprocessor include directives
- **Macro** - basic macro definitions (no conditional compilation)
- **Comment** - comment blocks with annotation extraction and entity association

**Specialized Parsers:**
- **ClassParser** - parses class definitions, inheritance hierarchies, template parameters, access specifiers, analyzes brace initialization eligibility
- **FunctionParser** - parses function signatures, return types, parameters, template parameters
- **ConstructorParser** - parses constructor parameter lists for dependency injection
- **DestructorParser** - parses destructor definitions (completeness, not used for DI)
- **ParameterParser** - parses function/constructor parameters with types, names, default values
- **EnumParser** - parses enum and enum class definitions with values
- **VariableParser** - parses variable declarations and types
- **TypeParser** - parses type expressions with qualifiers, templates, pointers, references, handles std:: type inference
- **ValueParser** - parses literal values, expressions, initializers (strings, numbers, calls, lists)
- **StatementParser** - base parser for all statement types
- **SimpleStatementParser** - parses single executable code lines
- **IfStatementParser** - parses if/else conditional blocks
- **ForLoopParser** - parses for loop constructs
- **WhileLoopParser** - parses while loop constructs
- **SwitchStatementParser** - parses switch/case constructs
- **NamespaceParser** - parses namespace scopes and qualified names
- **TypedefParser** - parses type aliases and definitions
- **UsingParser** - parses using declarations and using directives for type resolution
- **IncludeParser** - parses include statements for dependency tracking
- **MacroParser** - parses basic macro definitions for type expansion
- **CommentParser** - parses comments for annotations and associates with following entities

**Conditional Compilation Support:**
- Supports conditional compilation (`#if`, `#ifdef`, `#ifndef`, `#else`, `#elif`, `#endif`)
- Processes both unconditional macros (`#define`) and conditional directives
- Resolves conditional blocks using macro definitions from ParseContext
- Includes/excludes code blocks based on macro evaluation results

**Macro Expansion for Type Resolution:**
- When encountering a macro in type position, recursively expand using macro definitions
- Build macro symbol table during file parsing phase
- For constructor parameter `MyMacro param`, expand `MyMacro` to its definition
- Support nested macro expansion (macros that reference other macros)
- Error if macro expansion results in non-type construct or circular reference
- Example: `#define StringType std::string` → `StringType param` becomes `std::string param`

**Comment Association and Annotation Extraction:**
- **CommentParser** extracts AUTOWIRE/PROVIDER annotations from comment blocks
- Associates comments with immediately following code entities (Class, Function, Variable)
- Supports both single-line (`//`) and multi-line (`/* */`) comment styles
- Extracts descriptive text for generating CLI help messages and parameter descriptions
- Example: `/** Camera focal length in meters PROVIDER */` associates with following function
- Enables rich CLI documentation from source code comments

**Standard Library Type Handling:**
- **Initial Inference** - when TypeParser encounters `std::string` or `string` (via using), create Type with no source_construct
- **Default Assumption** - assume std:: types have no-args constructors for dependency injection
- **Progressive Learning** - refine std:: type knowledge by observing Variable declarations and Constructor parameters
- **Using Directive Resolution** - `using std::string` makes `string` resolve to `std::string` Type
- **Template Argument Extraction** - `std::vector<MyClass>` creates Type with template_args=[MyClass_Type]
- **Constructor Inference** - seeing `std::string name("value")` confirms std::string has string constructor
- **Parameter Pattern Learning** - seeing `MyClass(const std::string& name)` learns std::string is typically passed by const reference

**Standard Library Type Strategy:**
```cpp
// Initial encounter - create Type with default assumptions
std::string filename;  // → Type(qualified_name="std::string", source_construct=None, has_default_constructor=True)

// Learn from usage patterns
std::string name("test");           // → learns string constructor exists
std::vector<int> numbers{1,2,3};    // → learns initializer_list constructor
MyClass(const std::string& file);   // → learns typical parameter passing style

// Using directive resolution
using std::string;
string filename;  // → resolves to same std::string Type via alias_table
```

**Type Learning Process:**
1. **First Encounter** - create Type with qualified_name, no source_construct, assume default constructor
2. **Variable Declarations** - observe initialization patterns to learn available constructors
3. **Function Parameters** - learn typical parameter passing conventions (const&, value, pointer)
4. **Template Usage** - extract template arguments and create dependent Type objects
5. **Using Resolution** - map unqualified names to std:: types via alias_table
6. **Constructor Validation** - during dependency analysis, verify std:: types can be constructed as needed

## FileParser - Recursive Descent Parser

**Enhanced ParseContext with Parsing Utilities:**
- `match_regex(pattern: str) -> Optional[str]` - match regex at current position and advance
- `parse_identifier() -> Optional[str]` - parse C++ identifier at current position
- `skip_whitespace()` - skip whitespace and comments
- `peek(offset: int = 0) -> str` - look ahead without advancing position
- `save_position() -> int` / `restore_position(pos: int)` - backtracking support
- `find_matching_brace() -> int` - find matching closing brace
- `extract_braced_content() -> Optional[str]` - extract content between matching braces

**FileParser functions:**
- `parse_file(content: str, file_path: str) -> ParsedFile` - main parsing entry point using recursive descent
- `parse_next_construct(context: ParseContext) -> Optional[Construct]` - try each specialized parser with backtracking
- `try_parsers(context: ParseContext, parsers: List[Callable]) -> Optional[Construct]` - dispatch to specialized parsers

**ParsedFile dataclass:**
- `file_path: str` - absolute path to source file
- `constructs: List[Tuple[Construct, int]]` - all parsed constructs with line numbers in order
- `classes: List[Tuple[Class, int]]` - pre-filtered class/struct constructs with line numbers
- `functions: List[Tuple[Function, int]]` - pre-filtered function constructs with line numbers
- `constructors: List[Tuple[Constructor, int]]` - pre-filtered constructor constructs with line numbers
- `variables: List[Tuple[Variable, int]]` - pre-filtered variable constructs with line numbers
- `includes: List[Tuple[Include, int]]` - pre-filtered include constructs with line numbers
- `macros: List[Tuple[Macro, int]]` - pre-filtered macro constructs with line numbers
- `typedefs: List[Tuple[Typedef, int]]` - pre-filtered typedef constructs with line numbers
- `using_declarations: List[Tuple[Using, int]]` - pre-filtered using declarations with line numbers
- `destructors: List[Tuple[Destructor, int]]` - pre-filtered destructor constructs with line numbers
- `enums: List[Tuple[Enum, int]]` - pre-filtered enum constructs with line numbers
- `namespaces: List[Tuple[Namespace, int]]` - pre-filtered namespace constructs with line numbers
- `comments: List[Tuple[Comment, int]]` - pre-filtered standalone comment constructs with line numbers

**Construct Instance Fields:**
- **Comments** are attached as `comment: Optional[Comment]` field on Class, Function, Variable constructs
- **Namespaces** are attached as `namespace: Optional[str]` field on Class, Function, Variable constructs
- **Templates** - Class and Function constructs have `template_parameters: List[str]` field for template support
- **Access Specifiers** - Class constructs track public/private/protected sections for member visibility
- **Brace Initialization** - Class constructs have `can_brace_initialize: bool` and `brace_init_members: List[Variable]` fields
- **Source File** - all constructs have `file_path: str` field indicating their source file
- **Type Information** - Variable, Function (return type), Constructor (parameter types) contain Type constructs for full type resolution
- **Dual Usage**: Namespace and Comment constructs exist both as standalone constructs AND embedded context
- **Comment Absorption**: Comments immediately preceding other constructs are absorbed into those constructs
- **Standalone Comments**: Comments not associated with following constructs remain as separate Comment constructs

**Enhanced ParseContext dataclass:**
- `content: str` - complete file content for parsing
- `pos: int` - current parsing position in content
- `current_comment: Optional[Comment]` - comment block preceding current construct
- `current_namespace: Optional[Namespace]` - active namespace construct
- `macro_table: Dict[str, Macro]` - macro name to Macro construct mapping
- `alias_table: Dict[str, Union[Typedef, Using]]` - type alias name to Typedef/Using construct mapping
- `type_definitions: Dict[str, Class]` - type name to Class/Struct construct mapping
- `function_definitions: Dict[str, Function]` - function name to Function construct mapping
- `global_variables: Dict[str, Variable]` - global variable name to Variable construct mapping
- `static_variables: Dict[str, Variable]` - static variable name to Variable construct mapping
- Plus parsing utility methods listed above


**Recursive Descent Parsing Process:**
1. **Initialize Context** - create ParseContext with file content, position=0, and command-line macros from `-D` flags
2. **Parse Loop** - while not at end of file:
   - Skip whitespace and comments
   - Try each specialized parser function in sequence
   - First successful parser returns construct and advances position
   - Failed parsers backtrack to saved position
3. **Parser Functions** - each specialized parser receives ParseContext and:
   - Uses context utilities (match_regex, parse_identifier, etc.)
   - Updates symbol tables (macro_table, type_definitions, etc.)
   - Handles brace matching with extract_braced_content()
   - Returns parsed construct or None if no match
4. **Backtracking** - failed parsers restore position using save_position()/restore_position()
5. **Process Conditionals** - resolve `#if`/`#ifdef`/`#ifndef` blocks using macro context during parsing
6. **Return ParsedFile** - complete representation with constructs containing embedded context

**Parser Function Contract:**
- Each specialized parser receives `(context: ParseContext)`
- Uses context.match_regex(), context.parse_identifier(), etc. for parsing
- Returns `Optional[Construct]` - parsed construct or None if no match
- Must update context symbol tables if parsing succeeds (consume comments, update namespace)
- Uses context.save_position()/restore_position() for backtracking
- Main parser tries functions until one returns non-None result

**Context Usage in Specialized Parser Functions:**
- **parse_class()** uses context utilities, attaches `context.current_comment` and `context.current_namespace` to Class, analyzes brace initialization eligibility
- **parse_function()** uses context utilities, attaches comment/namespace to Function, consumes comment
- **parse_namespace()** updates `context.current_namespace` for subsequent constructs
- **parse_comment()** updates `context.current_comment` for next construct to consume, or creates standalone Comment construct if not absorbed
- All parsers use `context.extract_braced_content()` for handling class bodies, function bodies, etc.

**ClassParser Brace Initialization Analysis:**
- **Aggregate Detection** - determines if class qualifies for brace initialization during parsing:
  - No user-declared constructors (or only defaulted/deleted)
  - No private/protected non-static data members
  - No virtual functions or virtual base classes
- **Member Extraction** - collects public data members in declaration order for `brace_init_members`
- **Sets Flags** - populates `Class.can_brace_initialize` and `Class.brace_init_members` during construction

**Brace Initialization Examples:**
```cpp
// ClassParser detects: can_brace_initialize=True, brace_init_members=[filename, timeout, enabled]
struct Config {
    std::string filename;
    int timeout;
    bool enabled;
};

// ClassParser detects: can_brace_initialize=False (has constructor)
class Service {
public:
    Service(const Config& config);
private:
    Config config_;
};
```

**Recursive Descent Benefits:**
- **Robust Brace Handling** - extract_braced_content() properly matches braces in all contexts
- **Backtracking** - failed parsers don't corrupt parsing state
- **Context Awareness** - parsers understand semantic context (variable vs function vs class)
- **Position Tracking** - ParseContext maintains exact file positions for error reporting
- **Expandable** - add new parser functions by registering them in dispatch list

# Dependency Analysis Phase

The FileParser already builds the complete dependency graph via ParseContext symbol tables. We simply consume this parsed structure.

## Dependency Extraction

**DependencyAnalyzer class:**
- `extract_autowire_classes(parsed_files: List[ParsedFile]) -> List[Class]` - find classes with AUTOWIRE annotations
- `extract_provider_functions(parsed_files: List[ParsedFile]) -> List[Function]` - find functions with PROVIDER annotations
- `get_constructor_dependencies(class_obj: Class) -> List[str]` - extract parameter types from class constructors

- `validate_dependency_graph(autowire_classes: List[Class], provider_functions: List[Function]) -> List[str]` - check for missing providers and circular dependencies
- `detect_circular_dependencies(dependency_graph: Dict[str, List[str]]) -> List[List[str]]` - find circular dependency chains using DFS

**Dependency Resolution:**
- **ParseContext Contains Graph** - type_definitions, function_definitions already built during parsing
- **Constructor Dependencies** - extract from Constructor.parameters for each AUTOWIRE class
- **Provider Matching** - match Constructor parameter types to PROVIDER function return types
- **Inheritance Support** - use Class.base_classes for polymorphic dependency injection
- **Validation Only** - check that all dependencies have corresponding providers



**Circular Dependency Detection:**
- **Build Dependency Graph** - create adjacency list of type → dependencies mapping
- **DFS Cycle Detection** - use depth-first search with visited/recursion stack to find cycles
- **Error Reporting** - throw detailed error with full circular dependency chain
- **Example**: `ServiceA → DatabaseConnection → ConnectionPool → ServiceA` detected and reported
- **Early Termination** - stop processing on first circular dependency found

**Example Analysis:**
```
MyService (AUTOWIRE) constructor needs: [DatabaseConnection, Logger]
├── DatabaseConnection provided by: createDatabaseConnection() (PROVIDER)
├── Logger provided by: createLogger() (PROVIDER)

DatabaseConnection (PROVIDER) constructor needs: [std::string, ConnectionPool]
├── std::string: built-in type, no provider needed
├── ConnectionPool provided by: createConnectionPool() (PROVIDER)
```

# Code Generation Phase

Reuse parser constructs as serializers to generate C++ dependency injection code.

## Construct Serialization

**All Construct objects implement serialization:**
- `to_cpp() -> str` - serialize construct back to valid C++ code
- `to_header() -> str` - serialize construct as header declaration
- `to_implementation() -> str` - serialize construct as implementation code

**Serialization Examples:**
```python
# Class construct serialization
class_obj = Class(name="MyService", constructor=Constructor(...))
header_code = class_obj.to_header()  # → "class MyService { ... };"
impl_code = class_obj.to_implementation()  # → constructor definitions

# Function construct serialization  
func_obj = Function(name="createLogger", return_type=Type("Logger"))
code = func_obj.to_cpp()  # → "Logger createLogger() { ... }"

# Parameter construct serialization
param_obj = Parameter(name="filename", type=Type("std::string"))
code = param_obj.to_cpp()  # → "const std::string& filename"

# Parameter with default value
param_with_default = Parameter(
    name="threshold", 
    type=Type("double"), 
    default_value=Value("0.5")
)
code = param_with_default.to_cpp()  # → "double threshold = 0.5"

# Variable with initialization
var_obj = Variable(
    name="config", 
    type=Type("Config"), 
    initial_value=Value("Config{\"default.json\", true}")
)
code = var_obj.to_cpp()  # → "Config config = Config{\"default.json\", true};"
```

## Code Generator

**CodeGenerator class:**
- `generate_factory_functions(autowire_classes: List[Class]) -> List[Function]` - create factory functions
- `generate_cli_parser(autowire_classes: List[Class]) -> Function` - create command-line parser
- `generate_provider_wiring(provider_functions: List[Function]) -> List[Function]` - create dependency wiring
- `write_generated_files(output_dir: str, constructs: List[Construct]) -> None` - write .hpp/.cpp files

**Generated Code Strategy:**
1. **Create New Constructs** - build Function/Class objects for generated factory methods
2. **Reuse Serialization** - call `construct.to_cpp()` to generate actual C++ code
3. **Maintain Consistency** - generated code uses same parsing/serialization logic as input code
4. **Template Support** - Type objects handle template serialization automatically

**Value Construct Examples:**
```python
# Different value types
string_val = Value('"hello world"')  # String literal
number_val = Value("42")             # Numeric literal  
bool_val = Value("true")             # Boolean literal
call_val = Value("createLogger()")   # Function call
list_val = Value("{1, 2, 3}")        # Initializer list
expr_val = Value("x + y * 2")        # Expression
var_ref = Value("global_config")     # Variable reference
member_ref = Value("obj.member")     # Member access
array_ref = Value("data[index]")     # Array/container access

# Usage in Variable declarations
var1 = Variable("name", Type("std::string"), Value('"default"'))
var2 = Variable("count", Type("int"), Value("0"))
var3 = Variable("items", Type("std::vector<int>"), Value("{1, 2, 3}"))
var4 = Variable("copy", Type("Config"), Value("original_config"))  # Reference another variable

# Usage in Parameter default values
param1 = Parameter("timeout", Type("int"), Value("5000"))
param2 = Parameter("enabled", Type("bool"), Value("true"))
param3 = Parameter("config", Type("Config"), Value("default_config"))  # Reference global variable
```

**Statement Base Class:**
```python
# Abstract base class - never instantiated directly
class Statement:
    def to_cpp(self) -> str:
        raise NotImplementedError("Subclasses must implement to_cpp()")

# Concrete implementations
class SimpleStatement(Statement):
    def __init__(self, code: str):
        self.code = code
    def to_cpp(self) -> str:
        return self.code

class IfStatement(Statement):
    def to_cpp(self) -> str:
        # Generate if/else C++ code
```

**Statement Usage - Polymorphic Collections:**
```python
# Function body can contain any Statement subclass
body: List[Statement] = [
    SimpleStatement("auto logger = createLogger();"),  # Raw string
    IfStatement(condition="config.isValid()", ...),     # Structured control flow
    SimpleStatement("return result;")                   # Raw string
]

# All serialize the same way
for stmt in body:
    cpp_code += stmt.to_cpp()  # Polymorphic call

# Control flow constructs using List[Statement]
if_stmt = IfStatement(
    condition="filename.empty()",
    then_body=[SimpleStatement('throw std::invalid_argument("filename required");')],
    else_if_clauses=[
        ("threshold < 0", [SimpleStatement('throw std::invalid_argument("threshold must be positive");')]),
        ("threshold > 1.0", [
            SimpleStatement('threshold = 1.0;'), 
            SimpleStatement('std::cout << "Clamping threshold to 1.0" << std::endl;')
        ])
    ],
    else_body=[SimpleStatement('std::cout << "Valid parameters" << std::endl;')]
)

for_loop = ForLoop(
    initialization="int i = 0",
    condition="i < providers.size()",
    increment="++i",
    body=[Statement("providers[i]->initialize();")]
)

switch_stmt = SwitchStatement(
    expression="algorithm_type",
    cases=[
        ("DISTANCE", [Statement("return createDistanceAlgorithm();")]),
        ("CALIBRATION", [Statement("return createCalibrationAlgorithm();")])
    ],
    default_case=[Statement('throw std::runtime_error("Unknown algorithm");')]
)
```

**Example Generated Factory with Control Flow:**
```python
# Create factory function with complex body
factory_func = Function(
    name="createMyService",
    return_type=Type("std::unique_ptr<MyService>"),
    parameters=[
        Parameter("filename", Type("std::string")),
        Parameter("threshold", Type("double"))
    ],
    body=[
        IfStatement(
            condition="filename.empty()",
            then_body=[Statement('throw std::invalid_argument("filename required");')],
            else_if_clauses=[
                ("threshold < 0", [Statement('throw std::invalid_argument("invalid threshold");')])
            ],
            else_body=[Statement('std::cout << "Creating service with valid params" << std::endl;')]
        ),
        Statement("auto logger = createLogger();"),
        Statement("return std::make_unique<MyService>(filename, threshold, logger);")
    ]
)

# Serialize to C++
generated_code = factory_func.to_cpp()
# → Complete C++ function with proper control flow
```

**Value Integration Pattern:**
- **Variable.initial_value** - `Optional[Value]` for variable initialization
- **Parameter.default_value** - `Optional[Value]` for parameter defaults
- **Statement expressions** - can contain Value objects for assignments and calls
- **Serialization** - Value.to_cpp() generates proper C++ literal/expression syntax

**Statement Collection Pattern:**
- **Function.body** - `List[Statement]` containing all statement types
- **Class methods** - same `List[Statement]` pattern for method bodies
- **Control flow bodies** - use `List[Statement]` for if_body, else_body, loop_body
- **Unified Serialization** - all Statement subclasses implement `to_cpp()` for consistent code generation
- **Automatic Bracing** - control flow constructs handle `{ }` generation when serializing statement lists code generation

**Benefits of Construct Reuse:**
- **Consistency** - generated code follows same patterns as parsed code
- **Type Safety** - Type objects ensure correct C++ type syntax
- **Template Support** - automatic handling of template parameters and arguments
- **Namespace Awareness** - generated code respects namespace context
- **Comment Preservation** - can attach generated documentation to Comment objects