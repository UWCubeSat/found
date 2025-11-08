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