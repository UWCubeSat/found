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
from src.parsing.constructs.definitions.classes import Class, Constructor, Destructor, AccessSection, Method, FriendDeclaration
from src.parsing.constructs.definitions.functions import Function, Parameter
from src.parsing.constructs.statements.variables import Variable
from src.parsing.constructs.types.types import Type
from test.common.constants.construct_constants import (
    SAMPLE_FILE_PATH, CLASS_NAME, BASE_CLASS_NAME, DERIVED_CLASS_NAME, FUNCTION_NAME, INT_TYPE, PARAMETER_NAME,
    PUBLIC_ACCESS, PRIVATE_ACCESS, PROTECTED_ACCESS, TEMPLATE_PARAM_T,
    EXPECTED_COUNT_0, EXPECTED_COUNT_1, EXPECTED_COUNT_2, EXPECTED_COUNT_3, EXPECTED_TRUE, EXPECTED_FALSE
, set_parent)


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
        return Class(name)
    
    def create_base_class(self, name=BASE_CLASS_NAME):
        """Create a base class for inheritance testing.
        
        Args:
            name: Name for the base class
            
        Returns:
            Class: Base class construct
        """
        return Class(name)
    
    def create_derived_class(self, name, base_classes):
        """Create a derived class with specified inheritance.
        
        Args:
            name: Name for the derived class
            base_classes: List of (access, base_class) tuples
            
        Returns:
            Class: Derived class construct
        """
        cls = Class(name)
        for access, base_class in base_classes:
            cls.add_base_class(access, base_class)
        return cls
    

    
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
        cls = self.create_simple_class("Person")
        
        # Add private fields
        private_field1 = set_parent(Variable("id", set_parent(Type("int"), self.file)), self.file)
        private_field2 = set_parent(Variable("ssn", set_parent(Type("string"), self.file)), self.file)
        cls.private.members = [private_field1, private_field2]
        
        # Add public fields
        public_field1 = set_parent(Variable("name", set_parent(Type("string"), self.file)), self.file)
        public_field2 = set_parent(Variable("age", set_parent(Type("int"), self.file)), self.file)
        public_field3 = set_parent(Variable("active", set_parent(Type("bool"), self.file)), self.file)
        cls.public.members = [public_field1, public_field2, public_field3]
        
        # Add protected field
        protected_field = set_parent(Variable("internal_state", set_parent(Type("int"), self.file)), self.file)
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
    
    def test_add_base_class_single(self):
        """Test adding a single base class updates relationships."""
        base_class = self.create_base_class("Base")
        derived_class = Class("Derived")
        
        derived_class.add_base_class("public", base_class)
        
        self.assertEqual(len(derived_class.base_classes), 1)
        self.assertEqual(derived_class.base_classes[0], ("public", base_class))
        self.assertIn(derived_class, base_class.subclasses)
    
    def test_add_base_class_multiple(self):
        """Test adding multiple base classes."""
        base1 = self.create_base_class("Base1")
        base2 = self.create_base_class("Base2")
        derived_class = Class("Derived")
        
        derived_class.add_base_class("public", base1)
        derived_class.add_base_class("private", base2)
        
        expected_bases = [("public", base1), ("private", base2)]
        self.assertEqual(derived_class.base_classes, expected_bases)
        self.assertIn(derived_class, base1.subclasses)
        self.assertIn(derived_class, base2.subclasses)
    
    def test_add_base_class_inherits_methods(self):
        """Test that adding base class inherits methods."""
        base_class = Class("Base")
        method = Method("test_method", set_parent(Type("void"), self.file))
        base_class.public.methods.append((None, method))
        
        derived_class = Class("Derived")
        derived_class.add_base_class("public", base_class)
        
        # Should inherit the method
        self.assertEqual(len(derived_class.public.methods), 1)
        self.assertEqual(derived_class.public.methods[0][1].name, "test_method")
    
    def test_add_access_section_public(self):
        """Test adding public access section sets parent correctly."""
        cls = Class("TestClass")
        new_section = AccessSection()
        
        cls.add_access_section("public", new_section)
        
        self.assertEqual(cls.public, new_section)
        self.assertEqual(new_section.parent, cls)
    
    def test_add_access_section_private(self):
        """Test adding private access section sets parent correctly."""
        cls = Class("TestClass")
        new_section = AccessSection()
        
        cls.add_access_section("private", new_section)
        
        self.assertEqual(cls.private, new_section)
        self.assertEqual(new_section.parent, cls)
    
    def test_add_access_section_protected(self):
        """Test adding protected access section sets parent correctly."""
        cls = Class("TestClass")
        new_section = AccessSection()
        
        cls.add_access_section("protected", new_section)
        
        self.assertEqual(cls.protected, new_section)
        self.assertEqual(new_section.parent, cls)
    
    def test_add_access_section_invalid_level(self):
        """Test adding access section with invalid level raises error."""
        cls = Class("TestClass")
        new_section = AccessSection()
        
        with self.assertRaises(ValueError) as context:
            cls.add_access_section("invalid", new_section)
        
        self.assertIn("Invalid access level: invalid", str(context.exception))
    
    def test_add_nested_class(self):
        """Test adding nested class to access section."""
        outer_class = Class("OuterClass")
        nested_class = Class("NestedClass")
        
        outer_class.public.add_nested_class(nested_class)
        
        self.assertEqual(len(outer_class.public.nested_classes), 1)
        self.assertEqual(outer_class.public.nested_classes[0], nested_class)
        self.assertEqual(nested_class.parent, outer_class)


class TestMethod(unittest.TestCase):
    """Test cases for Method construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
    
    def test_method_with_virtual_flags(self):
        """Test method with virtual, static, const, and pure virtual flags."""
        return_type = set_parent(Type("void"), self.file)
        method = Method("test_method", return_type, is_virtual=True, is_const=True, is_pure_virtual=True, is_static=True)
        
        self.assertEqual(method.name, "test_method")
        self.assertEqual(method.return_type, return_type)
        self.assertTrue(method.is_virtual)
        self.assertTrue(method.is_const)
        self.assertTrue(method.is_pure_virtual)
        self.assertTrue(method.is_static)
    
    def test_method_with_parameters(self):
        """Test method with multiple parameters."""
        return_type = set_parent(Type("int"), self.file)
        param1 = Parameter("x", set_parent(Type("int"), self.file))
        param2 = Parameter("y", set_parent(Type("double"), self.file))
        method = Method("calculate", return_type, parameters=[param1, param2])
        
        self.assertEqual(method.name, "calculate")
        self.assertEqual(len(method.parameters), 2)
        self.assertEqual(method.parameters[0].name, "x")
        self.assertEqual(method.parameters[1].name, "y")
    
    def test_method_inheritance_behavior(self):
        """Test method behavior in inheritance context."""
        base_class = Class("Base")
        derived_class = Class("Derived")
        
        # Add virtual method to base class
        return_type = set_parent(Type("void"), self.file)
        virtual_method = Method("process", return_type, is_virtual=True)
        base_class.public.methods.append((None, virtual_method))
        
        # Add base class to derived
        derived_class.add_base_class("public", base_class)
        
        # Should inherit the virtual method
        self.assertEqual(len(derived_class.public.methods), 1)
        inherited_method = derived_class.public.methods[0][1]
        self.assertEqual(inherited_method.name, "process")
        self.assertTrue(inherited_method.is_virtual)
    
    def test_pure_virtual_method_makes_class_abstract(self):
        """Test that pure virtual methods make class abstract."""
        cls = Class("AbstractClass")
        return_type = set_parent(Type("void"), self.file)
        pure_virtual_method = Method("abstract_method", return_type, is_pure_virtual=True)
        cls.public.methods.append((None, pure_virtual_method))
        
        self.assertTrue(cls.has_pure_virtual_methods())
        self.assertTrue(cls.is_abstract())
        self.assertFalse(cls.can_be_instantiated())
    
    def test_method_access_levels(self):
        """Test methods in different access levels."""
        cls = Class("TestClass")
        return_type = set_parent(Type("void"), self.file)
        
        # Add methods to different access levels
        public_method = Method("public_method", return_type)
        private_method = Method("private_method", return_type)
        protected_method = Method("protected_method", return_type)
        
        cls.public.methods.append((None, public_method))
        cls.private.methods.append((None, private_method))
        cls.protected.methods.append((None, protected_method))
        
        self.assertEqual(len(cls.public.methods), 1)
        self.assertEqual(len(cls.private.methods), 1)
        self.assertEqual(len(cls.protected.methods), 1)
        self.assertEqual(cls.public.methods[0][1].name, "public_method")
        self.assertEqual(cls.private.methods[0][1].name, "private_method")
        self.assertEqual(cls.protected.methods[0][1].name, "protected_method")
    
    def test_static_method(self):
        """Test static method properties."""
        return_type = set_parent(Type("int"), self.file)
        static_method = Method("get_count", return_type, is_static=True)
        
        self.assertTrue(static_method.is_static)
        self.assertFalse(static_method.is_virtual)  # Static methods can't be virtual
        self.assertFalse(static_method.is_const)    # Static methods can't be const


class TestConstructor(unittest.TestCase):
    """Test cases for Constructor construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.parent_class = Class(CLASS_NAME)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def create_parameter(self, name="param"):
        """Helper to create a parameter."""
        param_type = Type(INT_TYPE)
        return Parameter(name, param_type)
    
    def test_constructor_with_multiple_parameters(self):
        """Test constructor with multiple parameters."""
        param1 = self.create_parameter("id")
        param2 = Parameter("name", set_parent(Type("string"), self.file))
        param3 = Parameter("age", set_parent(Type("int"), self.file))
        parameters = [param1, param2, param3]
        constructor = Constructor(self.parent_class.name, None, parameters=parameters)
        
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
    
    def test_destructor_validation(self):
        """Test destructor name validation."""
        # Valid destructor
        destructor = Destructor("~TestClass", None)
        self.assertEqual(destructor.name, "~TestClass")
        
        # Invalid destructor should raise error
        with self.assertRaises(ValueError) as context:
            Destructor("InvalidName", None)
        
        self.assertIn("must have ~ in front", str(context.exception))
    


class TestAccessSection(unittest.TestCase):
    """Test cases for AccessSection construct."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.file = File(SAMPLE_FILE_PATH)
        self.parent_class = Class(CLASS_NAME)
    
    def tearDown(self):
        """Clean up test fixtures."""
        pass
    
    def test_access_section_parent_set(self):
        """Test that access sections have correct parent."""
        cls = Class("TestClass")
        
        self.assertEqual(cls.public.parent, cls)
        self.assertEqual(cls.private.parent, cls)
        self.assertEqual(cls.protected.parent, cls)
    
    def test_access_section_with_multiple_members(self):
        """Test access section with multiple member variables."""
        from src.parsing.constructs.statements.variables import Variable
        
        # Create multiple member variables
        member1 = set_parent(Variable("id", set_parent(Type("int"), self.file)), self.file)
        member2 = set_parent(Variable("name", set_parent(Type("string"), self.file)), self.file)
        member3 = set_parent(Variable("active", set_parent(Type("bool"), self.file)), self.file)
        members = [member1, member2, member3]
        
        access_section = AccessSection()
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
        return_type = set_parent(Type("void"), self.file)
        method1 = set_parent(Method("initialize", return_type), self.file)
        method2 = set_parent(Method("process", return_type), self.file)
        method3 = set_parent(Method("cleanup", return_type), self.file)
        methods = [(None, method1), (None, method2), (None, method3)]
        
        access_section = AccessSection()
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
    
    def test_add_friend_declaration(self):
        """Test adding friend declaration to access section."""
        cls = Class("TestClass")
        friend_class = Class("FriendClass")
        friend_decl = FriendDeclaration(target=friend_class)
        
        cls.public.add_friend_declaration(friend_decl)
        
        self.assertEqual(len(cls.public.friend_declarations), 1)
        self.assertEqual(cls.public.friend_declarations[0], friend_decl)
        self.assertEqual(friend_decl.parent, cls)
    
    def test_friend_function_declaration(self):
        """Test friend function declaration."""
        cls = Class("TestClass")
        friend_function = Function("debug_print", set_parent(Type("void"), self.file))
        friend_decl = FriendDeclaration(target=friend_function)
        
        cls.private.add_friend_declaration(friend_decl)
        
        self.assertEqual(len(cls.private.friend_declarations), 1)
        self.assertEqual(cls.private.friend_declarations[0].target, friend_function)
        self.assertEqual(friend_decl.parent, cls)
