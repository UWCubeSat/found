"""Custom annotation for automatic repr method generation."""

def to_string(cls):
    """Class decorator that automatically generates __repr__ method.
    
    Uses all instance attributes (fields) of the class for string representation.
    Handles circular references by limiting depth and showing counts for collections.
    
    Args:
        cls: The class to decorate
        
    Returns:
        The decorated class with __repr__ method
    """
    
    def __repr__(self):
        """Auto-generated repr method."""
        def repr_with_visited(obj, visited, depth=0):
            """Generate repr with cycle detection and depth limiting."""
            if depth > 2 or id(obj) in visited:
                return f"<{type(obj).__name__} object>"
            
            visited.add(id(obj))
            try:
                return repr_attributes(obj, visited, depth)
            finally:
                visited.discard(id(obj))
        
        def repr_attributes(obj, visited, depth):
            """Generate repr for all attributes."""
            class_name = type(obj).__name__
            attrs = []
            
            for key, value in obj.__dict__.items():
                repr_value = repr_value_func(value, visited, depth + 1)
                attrs.append(f"{key}={repr_value}")
            
            return f"{class_name}({', '.join(attrs)})"
        
        def repr_value_func(value, visited, depth):
            """Generate repr for a single value with cycle detection."""
            if hasattr(value, '__dict__') and not isinstance(value, (str, int, float, bool)):
                return repr_with_visited(value, visited, depth)
            elif isinstance(value, (list, tuple)):
                if len(value) > 3:
                    return f"{type(value).__name__}({len(value)} items)"
                elif value and hasattr(value[0], '__dict__'):
                    items = [repr_value_func(item, visited, depth) for item in value[:3]]
                    suffix = "..." if len(value) > 3 else ""
                    return f"{type(value).__name__}([{', '.join(items)}{suffix}])"
                else:
                    return repr(value)
            elif isinstance(value, dict):
                if len(value) > 3:
                    return f"dict({len(value)} items)"
                else:
                    items = []
                    for k, v in list(value.items())[:3]:
                        repr_v = repr_value_func(v, visited, depth)
                        items.append(f"'{k}': {repr_v}")
                    suffix = ", ..." if len(value) > 3 else ""
                    return f"{{{', '.join(items)}{suffix}}}"
            elif isinstance(value, str) and len(value) > 50:
                return f"'{value[:47]}...'"
            else:
                return repr(value)
        
        return repr_with_visited(self, set())
    
    # Add the method to the class
    cls.__repr__ = __repr__
    
    return cls