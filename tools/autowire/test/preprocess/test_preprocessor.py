"""Test C++ preprocessor functionality."""

import unittest
import tempfile
import os
import shutil
from src.preprocess.preprocessor.preprocessor import Preprocessor
from src.preprocess.preprocessor.parameterized_file_cache import ParameterizedFileCache
from src.preprocess.constructs.parameterized_file import ParameterizedFile
from src.preprocess.constructs.macros import DefineDirective, IncludeDirective
from src.discovery.project_cache import ProjectFileCache, FileInfo
from test.common.constants.preprocessor_test_constants import *

if 'unittest.util' in __import__('sys').modules:
    # Show full diff in self.assertEqual.
    __import__('sys').modules['unittest.util']._MAX_LENGTH = 999999999


class PreprocessorTest(unittest.TestCase):
    """Test C++ preprocessor capabilities."""
    
    def setUp(self):
        self.temp_dir = tempfile.mkdtemp()
        # Create directory structure before initializing preprocessor
        os.makedirs(os.path.join(self.temp_dir, "top"), exist_ok=True)
        os.makedirs(os.path.join(self.temp_dir, "mid"), exist_ok=True)
        os.makedirs(os.path.join(self.temp_dir, "mid", "leaf"), exist_ok=True)
        os.makedirs(os.path.join(self.temp_dir, "templates"), exist_ok=True)
        self.cache = ProjectFileCache(self.temp_dir, {}, [], [])
        self.preprocessor = Preprocessor(self.cache, [self.temp_dir])
    
    def tearDown(self):
        shutil.rmtree(self.temp_dir)
    
    def create_test_file(self, content, filename="test.cpp"):
        file_path = os.path.join(self.temp_dir, filename)
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, 'w') as f:
            f.write(content)
        file_info = FileInfo(file_path, content, False, False)
        self.cache.add_file(file_info)
        return file_path
    
    def make_expected_macros(self, *defines):
        return {d.name: d for d in defines}
    
    def test_missing_macro_definition(self):
        file_path = self.create_test_file(NO_MACRO_DEFINITION)
        result = self.preprocessor.preprocess_file(file_path, NO_MACRO_DEFINITION, None, {})
        expected = ParameterizedFile(file_path, EXPECTED_NO_MACRO_DEFINITION, {}, [])
        self.assertEqual(result, expected)
    
    def test_empty_macros(self):
        """Test empty macro definitions."""
        file_path = self.create_test_file(EMPTY_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, EMPTY_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(EMPTY_DEFINE, PLACEHOLDER_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_EMPTY_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_simple_object_macros(self):
        """Test basic object-like macro expansion."""
        file_path = self.create_test_file(SIMPLE_OBJECT_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, SIMPLE_OBJECT_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(PI_DEFINE, VERSION_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_SIMPLE_OBJECT_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_no_params_function_macros(self):
        """Test function-like macros with no parameters."""
        file_path = self.create_test_file(NO_PARAMS_FUNCTION_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, NO_PARAMS_FUNCTION_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(INIT_DEFINE, CLEANUP_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_NO_PARAMS_FUNCTION_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_simple_function_macros(self):
        """Test basic function-like macro expansion."""
        file_path = self.create_test_file(SIMPLE_FUNCTION_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, SIMPLE_FUNCTION_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(MAX_DEFINE, MIN_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_SIMPLE_FUNCTION_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_multi_parameter_macros(self):
        """Test macros with multiple parameters."""
        file_path = self.create_test_file(MULTI_PARAM_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, MULTI_PARAM_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(FUNC_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_MULTI_PARAM_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_nested_macro_invocation(self):
        """Test macro calls within macros arguments"""
        file_path = self.create_test_file(NESTED_MACRO_INVOCATION)
        result = self.preprocessor.preprocess_file(file_path, NESTED_MACRO_INVOCATION, None, {})
        expected_macros = self.make_expected_macros(MAX_DEFINE, ADD_DEFINE, DIVIDE_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_NESTED_INVOCATION, expected_macros, [])
        self.assertEqual(result, expected)
        
    def test_line_continuation_handling(self):
        """Test line continuation handling in macro definitions."""        
        file_path = self.create_test_file(LINE_CONTINUATION_CPP)
        result = self.preprocessor.preprocess_file(file_path, LINE_CONTINUATION_CPP, None, {})
        expected_macros = self.make_expected_macros(LONG_MACRO_DEFINE, MAX_LINE_CONT_DEFINE, PI_DEFINE, COMPLEX_DEFINE, ANOTHER_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_LINE_CONTINUATION_OUTPUT, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_conditional_compilation_debug(self):
        """Test conditional compilation with DEBUG flag enabled."""
        debug_macro = DefineDirective(DEBUG_FLAG, "1")
        file_path = self.create_test_file(CONDITIONAL_COMPILATION_CPP)
        result = self.preprocessor.preprocess_file(file_path, CONDITIONAL_COMPILATION_CPP, None, {DEBUG_FLAG: debug_macro})
        expected_macros = self.make_expected_macros(debug_macro, LOG_DEBUG_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_CONDITIONAL_DEBUG, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_conditional_compilation_no_debug(self):
        """Test conditional compilation with DEBUG flag disabled."""
        file_path = self.create_test_file(CONDITIONAL_COMPILATION_CPP)
        result = self.preprocessor.preprocess_file(file_path, CONDITIONAL_COMPILATION_CPP, None, {})
        expected_macros = self.make_expected_macros(LOG_EMPTY_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_CONDITIONAL_NO_DEBUG, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_nested_conditionals_both_features(self):
        """Test nested conditional compilation with both features enabled."""
        feature_a = DefineDirective(FEATURE_A_FLAG, "1")
        feature_b = DefineDirective(FEATURE_B_FLAG, "1")
        file_path = self.create_test_file(NESTED_CONDITIONALS_CPP)
        result = self.preprocessor.preprocess_file(file_path, NESTED_CONDITIONALS_CPP, None, {FEATURE_A_FLAG: feature_a, FEATURE_B_FLAG: feature_b})
        expected_macros = self.make_expected_macros(feature_a, feature_b, MODE_AB_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_NESTED_AB, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_nested_conditionals_feature_a_only(self):
        """Test nested conditional compilation with only FEATURE_A enabled."""
        file_path = self.create_test_file(NESTED_CONDITIONALS_CPP)
        result = self.preprocessor.preprocess_file(file_path, NESTED_CONDITIONALS_CPP, None, {FEATURE_A_FLAG: FEATURE_A_DEFINE})
        expected_macros = self.make_expected_macros(FEATURE_A_DEFINE, MODE_A_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_NESTED_A_ONLY, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_nested_conditionals_no_features(self):
        """Test nested conditional compilation with no features enabled."""
        file_path = self.create_test_file(NESTED_CONDITIONALS_CPP)
        result = self.preprocessor.preprocess_file(file_path, NESTED_CONDITIONALS_CPP, None, {})
        expected_macros = self.make_expected_macros(MODE_NONE_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_NESTED_NONE, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_undef_and_redefine_macro(self):
        """Test macro undefinition and redefinition."""
        file_path = self.create_test_file(UNDEF_MACRO_CPP)
        result = self.preprocessor.preprocess_file(file_path, UNDEF_MACRO_CPP, None, {})
        expected_macros = self.make_expected_macros(TEMP_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_UNDEF_MACRO, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_stringification_operator(self):
        """Test stringification operator (#) functionality."""
        file_path = self.create_test_file(STRINGIFICATION_ONLY_CPP)
        result = self.preprocessor.preprocess_file(file_path, STRINGIFICATION_ONLY_CPP, None, {})
        expected_macros = self.make_expected_macros(STR_DEFINE, QUOTE_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_STRINGIFICATION_ONLY, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_token_pasting_operator(self):
        """Test token pasting operator (##) functionality."""
        file_path = self.create_test_file(TOKEN_PASTING_ONLY_CPP)
        result = self.preprocessor.preprocess_file(file_path, TOKEN_PASTING_ONLY_CPP, None, {})
        expected_macros = self.make_expected_macros(CONCAT_DEFINE, MAKE_VAR_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_TOKEN_PASTING_ONLY, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_token_pasting_and_stringification(self):
        """Test C++ preprocessor token pasting (##) and stringification (#) operators."""
        file_path = self.create_test_file(TOKEN_PASTING_CPP)
        result = self.preprocessor.preprocess_file(file_path, TOKEN_PASTING_CPP, None, {})
        expected_macros = self.make_expected_macros(CONCAT_DEFINE, STRINGIFY_DEFINE, QUOTE_PARAM_DEFINE, PREFIX_DEFINE, SUFFIX_DEFINE, MIXED1_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_TOKEN_PASTING_OUTPUT, expected_macros, [])
        self.assertEqual(result, expected)
    
    def test_dependency_injection_framework(self):
        """Test a complex dependency injection framework with nested macros."""
        cpp_content = DEPENDENCY_INJECTION_CPP
        
        # Test with DEBUG_MODE enabled
        file_path = self.create_test_file(cpp_content)
        
        result = self.preprocessor.preprocess_file(file_path, cpp_content, None, {DEBUG_MODE_FLAG: DEBUG_MODE_DEFINE})
        expected_macros = self.make_expected_macros(DEBUG_MODE_DEFINE, INJECT_SERVICE_DEFINE, REGISTER_SERVICE_DEFINE, SINGLETON_DEFINE, LOG_DEBUG_MSG_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_DEPENDENCY_INJECTION_DEBUG, expected_macros, [])
        self.assertEqual(result, expected)
        
        # Test with DEBUG_MODE disabled
        result_no_debug = self.preprocessor.preprocess_file(file_path, cpp_content, None, {})
        expected_macros_no_debug = self.make_expected_macros(INJECT_SERVICE_DEFINE, REGISTER_SERVICE_DEFINE, SINGLETON_DEFINE, LOG_NO_OP_DEFINE)
        expected_no_debug = ParameterizedFile(file_path, EXPECTED_DEPENDENCY_INJECTION_NO_DEBUG, expected_macros_no_debug, [])
        self.assertEqual(result_no_debug, expected_no_debug)
    
    def test_template_metaprogramming_with_macros(self):
        """Test complex template metaprogramming with conditional compilation."""
        cpp_content = TEMPLATE_METAPROGRAMMING_CPP
        
        file_path = self.create_test_file(cpp_content, "processor.cpp")
        
        # Test with both features enabled
        result = self.preprocessor.preprocess_file(file_path, cpp_content, None, {
            FEATURE_ASYNC_FLAG: FEATURE_ASYNC_DEFINE,
            FEATURE_CACHE_FLAG: FEATURE_CACHE_DEFINE
        })
        expected_macros = self.make_expected_macros(FEATURE_ASYNC_DEFINE, FEATURE_CACHE_DEFINE, ENABLE_FEATURE_DEFINE, DISABLE_FEATURE_DEFINE, ASYNC_METHOD_DEFINE, CACHED_RESULT_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_TEMPLATE_METAPROGRAMMING_ENABLED, expected_macros, [])
        self.assertEqual(result, expected)
        
        # Test with features disabled
        result_disabled = self.preprocessor.preprocess_file(file_path, cpp_content, None, {})
        expected_macros_disabled = self.make_expected_macros(ENABLE_FEATURE_DEFINE, DISABLE_FEATURE_DEFINE, ASYNC_METHOD_DISABLED_DEFINE, CACHED_RESULT_DISABLED_DEFINE)
        expected_disabled = ParameterizedFile(file_path, EXPECTED_TEMPLATE_METAPROGRAMMING_DISABLED, expected_macros_disabled, [])
        self.assertEqual(result_disabled, expected_disabled)
    
    def test_multi_level_macro_expansion(self):
        """Test complex multi-level macro expansion with token concatenation."""
        cpp_content = MULTI_LEVEL_MACRO_CPP
        
        file_path = self.create_test_file(cpp_content, "config.cpp")
        
        # Test with validation enabled
        result = self.preprocessor.preprocess_file(file_path, cpp_content, None, {USE_VALIDATION_FLAG: USE_VALIDATION_DEFINE})
        expected_macros = self.make_expected_macros(USE_VALIDATION_DEFINE, CONCAT_DEFINE, MAKE_GETTER_DEFINE, MAKE_SETTER_DEFINE, PROPERTY_DEFINE, SMART_POINTER_DEFINE, VALIDATE_INPUT_DEFINE)
        expected = ParameterizedFile(file_path, EXPECTED_MULTI_LEVEL_MACRO_VALIDATION, expected_macros, [])
        self.assertEqual(result, expected)
        
        # Test without validation
        result_no_validation = self.preprocessor.preprocess_file(file_path, cpp_content, None, {})
        expected_macros_no_validation = self.make_expected_macros(CONCAT_DEFINE, MAKE_GETTER_DEFINE, MAKE_SETTER_DEFINE, PROPERTY_DEFINE, SMART_POINTER_DEFINE, VALIDATE_INPUT_DEFINE)
        expected_no_validation = ParameterizedFile(file_path, EXPECTED_MULTI_LEVEL_MACRO_NO_VALIDATION, expected_macros_no_validation, [])
        self.assertEqual(result_no_validation, expected_no_validation)
        
        # Check that file defines its own macros
        macro_names = [m.name for m in result.defined_macros.values()]
        self.assertEqual(("CONCAT" in macro_names, "PROPERTY" in macro_names, "SMART_POINTER" in macro_names), (True, True, True))
    
    def test_include_at_beginning(self):
        """Test #include directives at the beginning of files."""
        header_path = self.create_test_file(HEADER_CONTENT, "header.hpp")
        main_path = self.create_test_file(INCLUDE_AT_BEGINNING_CPP, "main.cpp")
        result = self.preprocessor.get_translation_unit(main_path)
        expected = ParameterizedFileCache()
        header_file = ParameterizedFile(header_path, EXPECTED_HEADER_PROCESSED, {"HEADER_MACRO": HEADER_MACRO_DEFINE, "HEADER_HPP": HEADER_HPP_DEFINE}, [])
        main_file = ParameterizedFile(main_path, EXPECTED_INCLUDE_AT_BEGINNING, {"HEADER_MACRO": HEADER_MACRO_DEFINE, "HEADER_HPP": HEADER_HPP_DEFINE}, [IncludeDirective(header_file)])
        expected.put_file(header_file, [])
        expected.put_file(main_file, [])
        self.assertEqual(result, expected)
    
    def test_include_inside_code(self):
        """Test #include directives inside actual code blocks."""
        util_path = self.create_test_file(UTIL_CONTENT, "util.hpp")
        main_path = self.create_test_file(INCLUDE_INSIDE_CODE_CPP, "main.cpp")
        result = self.preprocessor.get_translation_unit(main_path)
        
        expected = ParameterizedFileCache()
        util_file = ParameterizedFile(util_path, "inline int add(int a, int b) { return a + b; }", {"UTIL_HPP": UTIL_HPP_DEFINE}, [])
        main_file = ParameterizedFile(main_path, EXPECTED_INCLUDE_INSIDE_CODE, {"UTIL_HPP": UTIL_HPP_DEFINE}, [])
        expected.put_file(util_file, [])
        expected.put_file(main_file, [])
        self.assertEqual(result, expected)
    
    def test_comprehensive_nested_includes(self):
        """Test complex nested include hierarchy with multiple levels and shared dependencies."""
        # Create all files in proper directory structure
        leaf1_path = self.create_test_file(LEAF1_CONTENT, "mid/leaf/leaf1.hpp")
        leaf2_path = self.create_test_file(LEAF2_CONTENT, "mid/leaf/leaf2.hpp")
        leaf3_path = self.create_test_file(LEAF3_CONTENT, "mid/leaf/leaf3.hpp")
        leaf4_path = self.create_test_file(LEAF4_CONTENT, "mid/leaf/leaf4.hpp")
        leaf5_path = self.create_test_file(LEAF5_CONTENT, "mid/leaf/leaf5.hpp")
        mid1_path = self.create_test_file(MID1_CONTENT, "mid/mid1.hpp")
        mid2_path = self.create_test_file(MID2_CONTENT, "mid/mid2.hpp")
        mid3_path = self.create_test_file(MID3_CONTENT, "mid/mid3.hpp")
        top_path = self.create_test_file(TOP_CONTENT, "top/top.hpp")
        main_path = self.create_test_file(MAIN_NESTED_CONTENT, "main.cpp")
        
        result = self.preprocessor.get_translation_unit(main_path)
        expected = ParameterizedFileCache()
        
        # Leaf files (no includes)
        leaf1_file = ParameterizedFile(leaf1_path, EXPECTED_LEAF1_PROCESSED, {"LEAF1_MACRO": LEAF1_MACRO_DEFINE, "LEAF1_HPP": LEAF1_HPP_DEFINE}, [])
        leaf2_file = ParameterizedFile(leaf2_path, EXPECTED_LEAF2_PROCESSED, {"LEAF2_MACRO": LEAF2_MACRO_DEFINE, "LEAF2_HPP": LEAF2_HPP_DEFINE}, [])
        leaf3_file = ParameterizedFile(leaf3_path, EXPECTED_LEAF3_PROCESSED, {"LEAF3_MACRO": LEAF3_MACRO_DEFINE, "LEAF3_HPP": LEAF3_HPP_DEFINE}, [])
        leaf4_file = ParameterizedFile(leaf4_path, EXPECTED_LEAF4_PROCESSED, {"LEAF4_MACRO": LEAF4_MACRO_DEFINE, "LEAF4_HPP": LEAF4_HPP_DEFINE}, [])
        leaf5_file = ParameterizedFile(leaf5_path, EXPECTED_LEAF5_PROCESSED, {"LEAF5_MACRO": LEAF5_MACRO_DEFINE, "LEAF5_HPP": LEAF5_HPP_DEFINE}, [])
        
        # Mid level files (include leaf files)
        mid1_file = ParameterizedFile(mid1_path, EXPECTED_MID1_PROCESSED, 
            {"MID1_MACRO": MID1_MACRO_DEFINE, "MID1_HPP": MID1_HPP_DEFINE, "LEAF1_MACRO": LEAF1_MACRO_DEFINE, "LEAF1_HPP": LEAF1_HPP_DEFINE, "LEAF2_MACRO": LEAF2_MACRO_DEFINE, "LEAF2_HPP": LEAF2_HPP_DEFINE},
            [IncludeDirective(leaf1_file), IncludeDirective(leaf2_file)])
        
        mid2_file = ParameterizedFile(mid2_path, EXPECTED_MID2_PROCESSED,
            {"MID2_MACRO": MID2_MACRO_DEFINE, "MID2_HPP": MID2_HPP_DEFINE, "LEAF3_MACRO": LEAF3_MACRO_DEFINE, "LEAF3_HPP": LEAF3_HPP_DEFINE},
            [IncludeDirective(leaf3_file)])
        
        mid3_file = ParameterizedFile(mid3_path, EXPECTED_MID3_PROCESSED,
            {"MID3_MACRO": MID3_MACRO_DEFINE, "MID3_HPP": MID3_HPP_DEFINE, "LEAF4_MACRO": LEAF4_MACRO_DEFINE, "LEAF4_HPP": LEAF4_HPP_DEFINE, "LEAF5_MACRO": LEAF5_MACRO_DEFINE, "LEAF5_HPP": LEAF5_HPP_DEFINE, "LEAF1_MACRO": LEAF1_MACRO_DEFINE, "LEAF1_HPP": LEAF1_HPP_DEFINE},
            [IncludeDirective(leaf4_file), IncludeDirective(leaf5_file), IncludeDirective(leaf1_file)])
        
        # Top level file (includes mid files)
        top_file = ParameterizedFile(top_path, EXPECTED_TOP_PROCESSED,
            {"TOP_MACRO": TOP_MACRO_DEFINE, "TOP_HPP": TOP_HPP_DEFINE, "MID1_MACRO": MID1_MACRO_DEFINE, "MID1_HPP": MID1_HPP_DEFINE, "MID2_MACRO": MID2_MACRO_DEFINE, "MID2_HPP": MID2_HPP_DEFINE, "MID3_MACRO": MID3_MACRO_DEFINE, "MID3_HPP": MID3_HPP_DEFINE,
             "LEAF1_MACRO": LEAF1_MACRO_DEFINE, "LEAF1_HPP": LEAF1_HPP_DEFINE, "LEAF2_MACRO": LEAF2_MACRO_DEFINE, "LEAF2_HPP": LEAF2_HPP_DEFINE, "LEAF3_MACRO": LEAF3_MACRO_DEFINE, "LEAF3_HPP": LEAF3_HPP_DEFINE, 
             "LEAF4_MACRO": LEAF4_MACRO_DEFINE, "LEAF4_HPP": LEAF4_HPP_DEFINE, "LEAF5_MACRO": LEAF5_MACRO_DEFINE, "LEAF5_HPP": LEAF5_HPP_DEFINE},
            [IncludeDirective(mid1_file), IncludeDirective(mid2_file), IncludeDirective(mid3_file)])
        
        # Main file (includes top file)
        main_file = ParameterizedFile(main_path, EXPECTED_MAIN_NESTED_PROCESSED,
            {"TOP_MACRO": TOP_MACRO_DEFINE, "TOP_HPP": TOP_HPP_DEFINE, "MID1_MACRO": MID1_MACRO_DEFINE, "MID1_HPP": MID1_HPP_DEFINE, "MID2_MACRO": MID2_MACRO_DEFINE, "MID2_HPP": MID2_HPP_DEFINE, "MID3_MACRO": MID3_MACRO_DEFINE, "MID3_HPP": MID3_HPP_DEFINE,
             "LEAF1_MACRO": LEAF1_MACRO_DEFINE, "LEAF1_HPP": LEAF1_HPP_DEFINE, "LEAF2_MACRO": LEAF2_MACRO_DEFINE, "LEAF2_HPP": LEAF2_HPP_DEFINE, "LEAF3_MACRO": LEAF3_MACRO_DEFINE, "LEAF3_HPP": LEAF3_HPP_DEFINE,
             "LEAF4_MACRO": LEAF4_MACRO_DEFINE, "LEAF4_HPP": LEAF4_HPP_DEFINE, "LEAF5_MACRO": LEAF5_MACRO_DEFINE, "LEAF5_HPP": LEAF5_HPP_DEFINE},
            [IncludeDirective(top_file)])
        
        # Add all files to expected cache
        expected.put_file(leaf1_file, [])
        expected.put_file(leaf2_file, [])
        expected.put_file(leaf3_file, [])
        expected.put_file(leaf4_file, [])
        expected.put_file(leaf5_file, [])
        expected.put_file(mid1_file, [])
        expected.put_file(mid2_file, [])
        expected.put_file(mid3_file, [])
        expected.put_file(top_file, [])
        expected.put_file(main_file, [])
        
        self.assertEqual(result, expected)
    
    def test_metaprogramming_inline_includes(self):
        """Test metaprogramming with includes inside code that get inlined."""
        # Create template files in templates directory
        member_path = self.create_test_file(TEMPLATE_MEMBER_CONTENT, "templates/member.hpp")
        getter_path = self.create_test_file(TEMPLATE_GETTER_CONTENT, "templates/getter.hpp")
        setter_path = self.create_test_file(TEMPLATE_SETTER_CONTENT, "templates/setter.hpp")
        main_path = self.create_test_file(METAPROGRAMMED_CLASS_CONTENT, "main.cpp")
        
        result = self.preprocessor.get_translation_unit(main_path)
        expected = ParameterizedFileCache()
        
        # Template files (no includes, no macros)
        member_file = ParameterizedFile(member_path, "T m_##name;", {"MEMBER_HPP": MEMBER_HPP_DEFINE}, [])
        getter_file = ParameterizedFile(getter_path, "T get##name() const { return m_##name; }", {"GETTER_HPP": GETTER_HPP_DEFINE}, [])
        setter_file = ParameterizedFile(setter_path, "void set##name(const T& value) { m_##name = value; }", {"SETTER_HPP": SETTER_HPP_DEFINE}, [])
        
        # Main file with inlined content (no include dependencies since they were inlined)
        main_file = ParameterizedFile(main_path, EXPECTED_METAPROGRAMMED_PROCESSED, {"MEMBER_HPP": MEMBER_HPP_DEFINE, "GETTER_HPP": GETTER_HPP_DEFINE, "SETTER_HPP": SETTER_HPP_DEFINE}, [])
        
        # Add all files to expected cache
        expected.put_file(member_file, [])
        expected.put_file(getter_file, [])
        expected.put_file(setter_file, [])
        expected.put_file(main_file, [])
        
        self.assertEqual(result, expected)
    
    def test_dependent_metaprogramming_inline_includes(self):
        """Test metaprogramming where inlined templates depend on macros from parent file."""
        # Create template files that depend on parent macro
        field_path = self.create_test_file(TEMPLATE_FIELD_CONTENT, "templates/field.hpp")
        accessor_path = self.create_test_file(TEMPLATE_ACCESSOR_CONTENT, "templates/accessor.hpp")
        parent_path = self.create_test_file(PARENT_CLASS_CONTENT, "parent.hpp")
        main_path = self.create_test_file(DEPENDENT_METAPROGRAMMING_CONTENT, "main.cpp")
        
        result = self.preprocessor.get_translation_unit(main_path)
        expected = ParameterizedFileCache()
        
        # Template files that depend on FIELD_TYPE macro
        field_file = ParameterizedFile(field_path, "int m_data;", {"FIELD_HPP": FIELD_HPP_DEFINE, "FIELD_TYPE": FIELD_TYPE_DEFINE}, [])
        accessor_file = ParameterizedFile(accessor_path, "int getData() const { return m_data; }\nvoid setData(const int& value) { m_data = value; }", {"ACCESSOR_HPP": ACCESSOR_HPP_DEFINE, "FIELD_TYPE": FIELD_TYPE_DEFINE}, [])
        
        # Parent file with inlined content and macro definitions
        parent_file = ParameterizedFile(parent_path, EXPECTED_PARENT_CLASS_PROCESSED, {"PARENT_HPP": PARENT_HPP_DEFINE, "FIELD_TYPE": FIELD_TYPE_DEFINE, "FIELD_HPP": FIELD_HPP_DEFINE, "ACCESSOR_HPP": ACCESSOR_HPP_DEFINE}, [])
        
        # Main file that includes parent
        main_file = ParameterizedFile(main_path, EXPECTED_DEPENDENT_METAPROGRAMMING_PROCESSED, {"PARENT_HPP": PARENT_HPP_DEFINE, "FIELD_TYPE": FIELD_TYPE_DEFINE, "FIELD_HPP": FIELD_HPP_DEFINE, "ACCESSOR_HPP": ACCESSOR_HPP_DEFINE}, [IncludeDirective(parent_file)])
        
        # Add all files to expected cache
        expected.put_file(field_file, [FIELD_TYPE_DEFINE])
        expected.put_file(accessor_file, [FIELD_TYPE_DEFINE])
        expected.put_file(parent_file, [])
        expected.put_file(main_file, [])
        
        self.assertEqual(result, expected)
