"""Tests for class construct classes.

This module tests Class-related construct functionality including:
- Class initialization with inheritance and template parameters
- Constructor and destructor handling
- Access section management (public, private, protected)
- Complex inheritance hierarchies (single, multiple, multi-level)
- Subclass relationship tracking and queries
"""

import unittest
from src.parsing.constructs.core.file import File
from src.parsing.constructs.definitions.classes import Class, Constructor, Destructor, AccessSection
from src.parsing.constructs.definitions.functions import Function, Parameter
from src.parsing.constructs.types.types import Type
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, CLASS_NAME, BASE_CLASS_NAME, DERIVED_CLASS_NAME, FUNCTION_NAME, INT_TYPE, PARAMETER_NAME,
    PUBLIC_ACCESS, PRIVATE_ACCESS, PROTECTED_ACCESS, TEMPLATE_PARAM_T,
    EXPECTED_COUNT_0, EXPECTED_COUNT_1, EXPECTED_COUNT_2, EXPECTED_COUNT_3, EXPECTED_TRUE, EXPECTED_FALSE
)


class TestClass(unittest.TestCase):
    """Test cases for Class construct functionality.
    
    Tests class definition, inheritance relationships, subclass tracking,
    and complex inheritance scenarios including multiple inheritance
    and multi-level hierarchies.
    """
    
    def setUp(self):
        """Set up test fixtures with file context."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_simple_class(self, name=CLASS_NAME):
        """Create a simple class with no inheritance.
        
        Args:
            name: Name for the class
            
        Returns:
            Class: Simple class construct
        """
        return Class(self.file, name)
    
    def create_base_class(self, name=BASE_CLASS_NAME):
        """Create a base class for inheritance testing.
        
        Args:
            name: Name for the base class
            
        Returns:
            Class: Base class construct
        """
        return Class(self.file, name)
    
    def create_derived_class(self, name, base_classes):
        """Create a derived class with specified inheritance.
        
        Args:
            name: Name for the derived class
            base_classes: List of (access, base_class) tuples
            
        Returns:
            Class: Derived class construct
        """
        return Class(self.file, name, base_classes=base_classes)
    
    def test_class_initialization_simple(self):
        """Test simple class initialization sets all properties and creates access sections."""
        cls = self.create_simple_class()
        expected_properties = {
            'name': CLASS_NAME,
            'is_struct': EXPECTED_FALSE,
            'base_classes_count': EXPECTED_COUNT_0,
            'template_params_count': EXPECTED_COUNT_0,
            'has_public': True,
            'has_private': True,
            'has_protected': True
        }
        actual_properties = {
            'name': cls.name,
            'is_struct': cls.is_struct,
            'base_classes_count': len(cls.base_classes),
            'template_params_count': len(cls.template_parameters),
            'has_public': cls.public is not None,
            'has_private': cls.private is not None,
            'has_protected': cls.protected is not None
        }
        
        self.assertEqual(actual_properties, expected_properties)
    
    def test_class_initialization_struct(self):
        """Test struct initialization."""
        expected_is_struct = True
        
        cls = Class(self.file, CLASS_NAME, is_struct=True)
        
        self.assertEqual(cls.is_struct, expected_is_struct)
    
    def test_class_inheritance_single(self):
        """Test single inheritance."""
        base_class = self.create_base_class()
        expected_base_classes = [(PUBLIC_ACCESS, base_class)]
        
        derived_class = self.create_derived_class("DerivedClass", expected_base_classes)
        
        self.assertEqual(derived_class.base_classes, expected_base_classes)
    
    def test_class_inheritance_multiple_bases(self):
        """Test multiple inheritance with different access levels."""
        base1 = self.create_base_class("Base1")
        base2 = self.create_base_class("Base2")
        base3 = self.create_base_class("Base3")
        expected_base_classes = [
            (PUBLIC_ACCESS, base1),
            (PRIVATE_ACCESS, base2),
            (PROTECTED_ACCESS, base3)
        ]
        
        derived_class = self.create_derived_class("MultiDerived", expected_base_classes)
        
        self.assertEqual(derived_class.base_classes, expected_base_classes)
    
    def test_class_inheritance_multilevel(self):
        """Test multi-level inheritance hierarchy."""
        # Create hierarchy: GrandParent -> Parent -> Child -> GrandChild
        grandparent = self.create_base_class("GrandParent")
        parent = self.create_derived_class("Parent", [(PUBLIC_ACCESS, grandparent)])
        child = self.create_derived_class("Child", [(PUBLIC_ACCESS, parent)])
        grandchild = self.create_derived_class("GrandChild", [(PUBLIC_ACCESS, child)])
        
        # Test each level has correct immediate parent
        expected_parent_bases = [(PUBLIC_ACCESS, grandparent)]
        expected_child_bases = [(PUBLIC_ACCESS, parent)]
        expected_grandchild_bases = [(PUBLIC_ACCESS, child)]
        
        self.assertEqual(parent.base_classes, expected_parent_bases)
        self.assertEqual(child.base_classes, expected_child_bases)
        self.assertEqual(grandchild.base_classes, expected_grandchild_bases)
    
    def test_add_subclass(self):
        """Test adding subclass relationship."""
        base_class = self.create_base_class()
        derived_class = self.create_derived_class("DerivedClass", [(PUBLIC_ACCESS, base_class)])
        expected_subclasses = [derived_class]
        
        base_class.add_subclass(derived_class)
        
        self.assertEqual(base_class.subclasses, expected_subclasses)
    
    def test_get_direct_subclasses(self):
        """Test getting direct subclasses."""
        base_class = self.create_base_class()
        derived1 = self.create_derived_class("Derived1", [(PUBLIC_ACCESS, base_class)])
        derived2 = self.create_derived_class("Derived2", [(PUBLIC_ACCESS, base_class)])
        expected_subclasses = [derived1, derived2]
        
        base_class.add_subclass(derived1)
        base_class.add_subclass(derived2)
        result = base_class.get_direct_subclasses()
        
        self.assertEqual(result, expected_subclasses)
    
    def test_get_all_subclasses_multilevel(self):
        """Test getting all subclasses in multilevel hierarchy."""
        base = self.create_base_class("Base")
        child1 = self.create_derived_class("Child1", [(PUBLIC_ACCESS, base)])
        child2 = self.create_derived_class("Child2", [(PUBLIC_ACCESS, base)])
        grandchild1 = self.create_derived_class("GrandChild1", [(PUBLIC_ACCESS, child1)])
        grandchild2 = self.create_derived_class("GrandChild2", [(PUBLIC_ACCESS, child1)])
        expected_all_subclasses = [child1, child2, grandchild1, grandchild2]
        
        base.add_subclass(child1)
        base.add_subclass(child2)
        child1.add_subclass(grandchild1)
        child1.add_subclass(grandchild2)
        result = base.get_all_subclasses()
        
        self.assertEqual(result, expected_all_subclasses)
    
    def test_can_be_instantiated_concrete(self):
        """Test concrete class without pure virtual methods can be instantiated."""
        cls = self.create_simple_class()
        
        result = cls.can_be_instantiated()
        
        self.assertEqual(result, EXPECTED_TRUE)
    
    def test_class_with_multiple_fields(self):
        """Test class with multiple fields across different access sections."""
        from src.parsing.constructs.statements.variables import Variable
        
        cls = self.create_simple_class("Person")
        
        # Add private fields
        private_field1 = Variable(self.file, "id", Type(self.file, "int"))
        private_field2 = Variable(self.file, "ssn", Type(self.file, "string"))
        cls.private.members = [private_field1, private_field2]
        
        # Add public fields
        public_field1 = Variable(self.file, "name", Type(self.file, "string"))
        public_field2 = Variable(self.file, "age", Type(self.file, "int"))
        public_field3 = Variable(self.file, "active", Type(self.file, "bool"))
        cls.public.members = [public_field1, public_field2, public_field3]
        
        # Add protected field
        protected_field = Variable(self.file, "internal_state", Type(self.file, "int"))
        cls.protected.members = [protected_field]
        
        expected = {
            'private_fields_count': 2,
            'public_fields_count': 3,
            'protected_fields_count': 1,
            'first_private_field': "id",
            'first_public_field': "name",
            'protected_field': "internal_state"
        }
        actual = {
            'private_fields_count': len(cls.private.members),
            'public_fields_count': len(cls.public.members),
            'protected_fields_count': len(cls.protected.members),
            'first_private_field': cls.private.members[0].name,
            'first_public_field': cls.public.members[0].name,
            'protected_field': cls.protected.members[0].name
        }
        
        self.assertEqual(expected, actual)


class TestConstructor(unittest.TestCase):
    """Test cases for Constructor construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.parent_class = Class(self.file, CLASS_NAME)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_parameter(self, name="param"):
        """Helper to create a parameter."""
        param_type = Type(self.file, INT_TYPE)
        return Parameter(self.file, name, param_type)
    
    def test_constructor_initialization_simple(self):
        """Test simple constructor initialization."""
        constructor = Constructor(self.parent_class)
        
        expected = {
            'parent': self.parent_class,
            'comments': [],
            'parameters': []
        }
        
        self.assertEqual(expected, constructor.__dict__)
    
    def test_constructor_with_parameters(self):
        """Test constructor with parameters."""
        param = self.create_parameter()
        parameters = [param]
        constructor = Constructor(self.parent_class, parameters=parameters)
        
        expected = {
            'parent': self.parent_class,
            'comments': [],
            'parameters': parameters
        }
        
        self.assertEqual(expected, constructor.__dict__)
    
    def test_constructor_with_multiple_parameters(self):
        """Test constructor with multiple parameters."""
        param1 = self.create_parameter("id")
        param2 = Parameter(self.file, "name", Type(self.file, "string"))
        param3 = Parameter(self.file, "age", Type(self.file, "int"))
        parameters = [param1, param2, param3]
        constructor = Constructor(self.parent_class, parameters=parameters)
        
        expected = {
            'parameters_count': 3,
            'first_param_name': "id",
            'second_param_name': "name",
            'third_param_name': "age"
        }
        actual = {
            'parameters_count': len(constructor.parameters),
            'first_param_name': constructor.parameters[0].name,
            'second_param_name': constructor.parameters[1].name,
            'third_param_name': constructor.parameters[2].name
        }
        
        self.assertEqual(expected, actual)


class TestDestructor(unittest.TestCase):
    """Test cases for Destructor construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.parent_class = Class(self.file, CLASS_NAME)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_destructor_initialization_simple(self):
        """Test simple destructor initialization."""
        destructor = Destructor(self.parent_class)
        
        expected = {
            'parent': self.parent_class,
            'comments': [],
            'is_virtual': False
        }
        
        self.assertEqual(expected, destructor.__dict__)
    
    def test_destructor_virtual(self):
        """Test virtual destructor initialization."""
        destructor = Destructor(self.parent_class, is_virtual=True)
        
        expected = {
            'parent': self.parent_class,
            'comments': [],
            'is_virtual': True
        }
        
        self.assertEqual(expected, destructor.__dict__)


class TestAccessSection(unittest.TestCase):
    """Test cases for AccessSection construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.parent_class = Class(self.file, CLASS_NAME)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_access_section_initialization(self):
        """Test access section initialization."""
        access_section = AccessSection(self.parent_class)
        
        expected = {
            'parent': self.parent_class,
            'comments': [],
            'constructors': [],
            'methods': [],
            'members': []
        }
        
        self.assertEqual(expected, access_section.__dict__)
    
    def test_access_section_with_multiple_members(self):
        """Test access section with multiple member variables."""
        from src.parsing.constructs.statements.variables import Variable
        
        # Create multiple member variables
        member1 = Variable(self.file, "id", Type(self.file, "int"))
        member2 = Variable(self.file, "name", Type(self.file, "string"))
        member3 = Variable(self.file, "active", Type(self.file, "bool"))
        members = [member1, member2, member3]
        
        access_section = AccessSection(self.parent_class)
        access_section.members = members
        
        expected = {
            'members_count': 3,
            'first_member_name': "id",
            'second_member_name': "name",
            'third_member_name': "active"
        }
        actual = {
            'members_count': len(access_section.members),
            'first_member_name': access_section.members[0].name,
            'second_member_name': access_section.members[1].name,
            'third_member_name': access_section.members[2].name
        }
        
        self.assertEqual(expected, actual)
    
    def test_access_section_with_multiple_methods(self):
        """Test access section with multiple methods."""
        # Create multiple methods
        return_type = Type(self.file, "void")
        method1 = Function(self.file, "initialize", return_type)
        method2 = Function(self.file, "process", return_type)
        method3 = Function(self.file, "cleanup", return_type)
        methods = [(None, method1), (None, method2), (None, method3)]
        
        access_section = AccessSection(self.parent_class)
        access_section.methods = methods
        
        expected = {
            'methods_count': 3,
            'first_method_name': "initialize",
            'second_method_name': "process",
            'third_method_name': "cleanup"
        }
        actual = {
            'methods_count': len(access_section.methods),
            'first_method_name': access_section.methods[0][1].name,
            'second_method_name': access_section.methods[1][1].name,
            'third_method_name': access_section.methods[2][1].name
        }
        
        self.assertEqual(expected, actual)


if __name__ == '__main__':
    unittest.main()