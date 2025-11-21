"""Tests for auto_init decorator."""

import unittest
from src.common.annotations.auto_properties import auto_init


class TestAutoInit(unittest.TestCase):
    """Test cases for auto_init decorator."""
    
    def test_required_fields(self):
        """Test constructor with required fields."""
        @auto_init(name=str, value=int)
        class TestClass:
            pass
        
        obj = TestClass(name="test", value=42)
        self.assertEqual(obj.name, "test")
        self.assertEqual(obj.value, 42)
    
    def test_optional_fields(self):
        """Test constructor with optional fields."""
        @auto_init(name=str, age=(int, 25), items=(list, None))
        class TestClass:
            pass
        
        obj = TestClass(name="John")
        self.assertEqual(obj.name, "John")
        self.assertEqual(obj.age, 25)
        self.assertIsNone(obj.items)
    
    def test_missing_required_field(self):
        """Test error when required field is missing."""
        @auto_init(name=str, value=int)
        class TestClass:
            pass
        
        with self.assertRaises(TypeError) as cm:
            TestClass(name="test")
        self.assertIn("Missing required argument: value", str(cm.exception))
    
    def test_override_defaults(self):
        """Test overriding default values."""
        @auto_init(name=str, age=(int, 25))
        class TestClass:
            pass
        
        obj = TestClass(name="John", age=30)
        self.assertEqual(obj.age, 30)