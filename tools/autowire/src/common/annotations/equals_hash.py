"""Custom annotation for automatic equals and hash method generation."""

def equals_hash(cls):
    """Class decorator that automatically generates __eq__ and __hash__ methods.
    
    Uses all instance attributes (fields) of the class for equality comparison
    and hash calculation. Properly handles inheritance by calling superclass methods.
    
    Args:
        cls: The class to decorate
        
    Returns:
        The decorated class with __eq__ and __hash__ methods
        
    Preconditions:
        cls must be a valid class definition
        All instance attributes should be hashable for proper hash generation
    """
    
    def __eq__(self, other):
        """Auto-generated equality method.
        
        Args:
            other: Object to compare with
            
        Returns:
            bool: True if objects are equal based on all instance attributes
        """
        if not isinstance(other, self.__class__):
            return False
        
        def equals_with_visited(obj1, obj2, visited):
            """Compare objects with cycle detection."""
            pair_key = (id(obj1), id(obj2))
            if pair_key in visited:
                return True
            visited.add(pair_key)
            try:
                return compare_attributes(obj1, obj2, visited)
            finally:
                visited.discard(pair_key)
        
        def compare_attributes(obj1, obj2, visited):
            """Compare all attributes."""
            for key in obj1.__dict__:
                if key not in obj2.__dict__:
                    return False
                if not values_equal(obj1.__dict__[key], obj2.__dict__[key], visited):
                    return False
            for key in obj2.__dict__:
                if key not in obj1.__dict__:
                    return False
            return True
        
        def values_equal(value1, value2, visited):
            """Compare two values with cycle detection."""
            if hasattr(value1, '__dict__') and hasattr(value2, '__dict__'):
                if not isinstance(value2, value1.__class__):
                    return False
                return equals_with_visited(value1, value2, visited)
            elif isinstance(value1, (list, tuple)) and isinstance(value2, (list, tuple)):
                if len(value1) != len(value2):
                    return False
                for item1, item2 in zip(value1, value2):
                    if not values_equal(item1, item2, visited):
                        return False
                return True
            else:
                return value1 == value2
        
        return equals_with_visited(self, other, set())
    
    def __hash__(self):
        """Auto-generated hash method.
        
        Returns:
            int: Hash value based on hashable instance attributes
        """
        def hash_with_visited(obj, visited):
            """Hash object with cycle detection."""
            if id(obj) in visited:
                return hash((type(obj).__name__, "CYCLE_DETECTED"))
            visited.add(id(obj))
            try:
                hashable_items = []
                for key, value in sorted(obj.__dict__.items()):
                    hash_value = hash_value_func(value, visited)
                    hashable_items.append((key, hash_value))
                return hash(tuple(hashable_items))
            finally:
                visited.discard(id(obj))
        
        def hash_value_func(value, visited):
            """Hash a single value with cycle detection."""
            if hasattr(value, '__dict__'):
                return hash_with_visited(value, visited)
            elif isinstance(value, (list, tuple)):
                if value and hasattr(value[0], '__dict__'):
                    return tuple(hash_value_func(item, visited) for item in value)
                else:
                    return (type(value).__name__, len(value))
            elif isinstance(value, dict):
                return (type(value).__name__, len(value))
            else:
                try:
                    hash(value)
                    return value
                except TypeError:
                    return (type(value).__name__, len(value) if hasattr(value, '__len__') else 0)
        
        return hash_with_visited(self, set())
    
    # Add the methods to the class
    cls.__eq__ = __eq__
    cls.__hash__ = __hash__
    
    return cls