"""Tests for equals_hash annotation."""

import unittest
from src.common.annotations import equals_hash

# Test constants
TEST_PERSON_ALICE = "Alice"
TEST_PERSON_BOB = "Bob"
TEST_AGE_30 = 30
TEST_AGE_25 = 25
TEST_ADDRESS_STREET = "123 Main St"
TEST_ADDRESS_CITY = "City"
TEST_FIELD_VALUE = "value"
TEST_FIELD_DIFFERENT = "different"
TEST_FIELD_NAME = "field"
EXPECTED_SET_SIZE = 2


class TestEqualsHash(unittest.TestCase):
    """Test cases for equals_hash annotation."""
    
    def setUp(self):
        """Set up test fixtures."""
        @equals_hash
        class Person:
            def __init__(self, name, age):
                self.name = name
                self.age = age
        
        @equals_hash
        class Address:
            def __init__(self, street, city):
                self.street = street
                self.city = city
        
        self.Person = Person
        self.Address = Address
    
    def test_equality_same_values(self):
        """Test equality with same field values."""
        p1 = self.Person(TEST_PERSON_ALICE, TEST_AGE_30)
        p2 = self.Person(TEST_PERSON_ALICE, TEST_AGE_30)
        
        expected = True
        actual = (p1 == p2)
        
        self.assertEqual(expected, actual)
    
    def test_equality_different_values(self):
        """Test equality with different field values."""
        p1 = self.Person("Alice", 30)
        p2 = self.Person("Bob", 25)
        
        expected = False
        actual = (p1 == p2)
        
        self.assertEqual(expected, actual)
    
    def test_equality_different_types(self):
        """Test equality with different class types."""
        person = self.Person("Alice", 30)
        address = self.Address("123 Main St", "City")
        
        expected = False
        actual = (person == address)
        
        self.assertEqual(expected, actual)
    
    def test_hash_same_values(self):
        """Test hash consistency with same field values."""
        p1 = self.Person("Alice", 30)
        p2 = self.Person("Alice", 30)
        
        expected = True
        actual = (hash(p1) == hash(p2))
        
        self.assertEqual(expected, actual)
    
    def test_hash_different_values(self):
        """Test hash difference with different field values."""
        p1 = self.Person("Alice", 30)
        p2 = self.Person("Bob", 25)
        
        expected = False
        actual = (hash(p1) == hash(p2))
        
        self.assertEqual(expected, actual)
    
    def test_set_deduplication(self):
        """Test that equal objects are deduplicated in sets."""
        p1 = self.Person("Alice", 30)
        p2 = self.Person("Alice", 30)
        p3 = self.Person("Bob", 25)
        
        people_set = {p1, p2, p3}
        
        expected = 2
        actual = len(people_set)
        
        self.assertEqual(expected, actual)
    
    def test_empty_object(self):
        """Test annotation works with objects that have no fields initially."""
        @equals_hash
        class Empty:
            pass
        
        e1 = Empty()
        e2 = Empty()
        
        expected = (True, True)
        actual = (e1 == e2, hash(e1) == hash(e2))
        
        self.assertEqual(expected, actual)
    
    def test_dynamic_fields(self):
        """Test annotation works when fields are added dynamically."""
        @equals_hash
        class Dynamic:
            pass
        
        d1 = Dynamic()
        d1.field = "value"
        
        d2 = Dynamic()
        d2.field = "value"
        
        d3 = Dynamic()
        d3.field = "different"
        
        expected = (True, False)
        actual = (d1 == d2, d1 == d3)
        
        self.assertEqual(expected, actual)
    
    def test_unhashable_list_attributes(self):
        """Test hash works with objects containing list attributes."""
        @equals_hash
        class WithList:
            def __init__(self, name, items):
                self.name = name
                self.items = items
        
        obj1 = WithList("test", [1, 2, 3])
        obj2 = WithList("test", [1, 2, 3])
        obj3 = WithList("test", [4, 5])
        
        # Should not raise TypeError
        hash1 = hash(obj1)
        hash2 = hash(obj2)
        hash3 = hash(obj3)
        
        expected_equality = True
        expected_different_hash = True
        
        actual_equality = (obj1 == obj2)
        actual_different_hash = (hash1 != hash3)
        
        self.assertEqual(expected_equality, actual_equality)
        self.assertEqual(expected_different_hash, actual_different_hash)
    
    def test_unhashable_dict_attributes(self):
        """Test hash works with objects containing dict attributes."""
        @equals_hash
        class WithDict:
            def __init__(self, name, data):
                self.name = name
                self.data = data
        
        obj1 = WithDict("test", {"key": "value"})
        obj2 = WithDict("test", {"key": "value"})
        
        # Should not raise TypeError
        hash1 = hash(obj1)
        hash2 = hash(obj2)
        
        expected_equality = True
        actual_equality = (obj1 == obj2)
        
        self.assertEqual(expected_equality, actual_equality)
    
    def test_circular_reference_objects(self):
        """Test hash works with objects that reference each other."""
        @equals_hash
        class Node:
            def __init__(self, name):
                self.name = name
                self.children = []
                self.parent = None
        
        parent = Node("parent")
        child1 = Node("child1")
        child2 = Node("child2")
        
        # Create circular references
        parent.children = [child1, child2]
        child1.parent = parent
        child2.parent = parent
        
        # Should not raise RecursionError
        hash_parent = hash(parent)
        hash_child1 = hash(child1)
        hash_child2 = hash(child2)
        
        expected_different_hashes = True
        actual_different_hashes = (hash_parent != hash_child1 != hash_child2)
        
        self.assertEqual(expected_different_hashes, actual_different_hashes)
    
    def test_list_with_objects(self):
        """Test hash works with lists containing objects with __dict__."""
        @equals_hash
        class Container:
            def __init__(self, name, objects):
                self.name = name
                self.objects = objects
        
        @equals_hash
        class Item:
            def __init__(self, value):
                self.value = value
        
        item1 = Item("a")
        item2 = Item("b")
        
        container1 = Container("test", [item1, item2])
        container2 = Container("test", [item1, item2])
        
        # Should not raise TypeError or RecursionError
        hash1 = hash(container1)
        hash2 = hash(container2)
        
        expected_equality = True
        actual_equality = (container1 == container2)
        
        self.assertEqual(expected_equality, actual_equality)
    
    def test_nested_objects_equality(self):
        """Test equality works with deeply nested objects."""
        @equals_hash
        class Address:
            def __init__(self, street, city):
                self.street = street
                self.city = city
        
        @equals_hash
        class Person:
            def __init__(self, name, address):
                self.name = name
                self.address = address
        
        @equals_hash
        class Company:
            def __init__(self, name, employees):
                self.name = name
                self.employees = employees
        
        # Create nested structure
        addr1 = Address("123 Main St", "City")
        addr2 = Address("123 Main St", "City")
        person1 = Person("Alice", addr1)
        person2 = Person("Alice", addr2)
        company1 = Company("TechCorp", [person1])
        company2 = Company("TechCorp", [person2])
        
        expected_equality = True
        expected_hash_equality = True
        
        actual_equality = (company1 == company2)
        actual_hash_equality = (hash(company1) == hash(company2))
        
        self.assertEqual(expected_equality, actual_equality)
        self.assertEqual(expected_hash_equality, actual_hash_equality)
    
    def test_nested_circular_references(self):
        """Test nested objects with circular references."""
        @equals_hash
        class Department:
            def __init__(self, name):
                self.name = name
                self.manager = None
                self.employees = []
        
        @equals_hash
        class Employee:
            def __init__(self, name, department):
                self.name = name
                self.department = department
        
        # Create circular structure: dept -> manager -> dept
        dept1 = Department("Engineering")
        manager1 = Employee("Bob", dept1)
        dept1.manager = manager1
        dept1.employees = [manager1]
        
        dept2 = Department("Engineering")
        manager2 = Employee("Bob", dept2)
        dept2.manager = manager2
        dept2.employees = [manager2]
        
        expected_equality = True
        expected_hash_equality = True
        
        # Should not raise RecursionError
        actual_equality = (dept1 == dept2)
        actual_hash_equality = (hash(dept1) == hash(dept2))
        
        self.assertEqual(expected_equality, actual_equality)
        self.assertEqual(expected_hash_equality, actual_hash_equality)
    
    def test_deep_nesting_levels(self):
        """Test very deep nesting levels."""
        @equals_hash
        class Node:
            def __init__(self, value, child=None):
                self.value = value
                self.child = child
        
        # Create deep nesting: node1 -> node2 -> node3 -> node4 -> node5
        node5a = Node(5)
        node4a = Node(4, node5a)
        node3a = Node(3, node4a)
        node2a = Node(2, node3a)
        node1a = Node(1, node2a)
        
        node5b = Node(5)
        node4b = Node(4, node5b)
        node3b = Node(3, node4b)
        node2b = Node(2, node3b)
        node1b = Node(1, node2b)
        
        expected_equality = True
        expected_hash_equality = True
        
        actual_equality = (node1a == node1b)
        actual_hash_equality = (hash(node1a) == hash(node1b))
        
        self.assertEqual(expected_equality, actual_equality)
        self.assertEqual(expected_hash_equality, actual_hash_equality)