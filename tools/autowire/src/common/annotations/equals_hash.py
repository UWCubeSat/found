"""Custom annotation for automatic equals and hash method generation."""




def equals_hash(cls):
    """Class decorator that automatically generates __eq__ and __hash__ methods.
    
    Uses all instance attributes (fields) of the class for equality comparison
    and hash calculation.
    
    Args:
        cls: The class to decorate
        
    Returns:
        The decorated class with __eq__ and __hash__ methods
    """
    
    def __eq__(self, other):
        """Auto-generated equality method."""
        if not isinstance(other, self.__class__):
            return False
        return self.__dict__ == other.__dict__
    
    def __hash__(self):
        """Auto-generated hash method."""
        return hash(tuple(sorted(self.__dict__.items())))
    
    # Add the methods to the class
    cls.__eq__ = __eq__
    cls.__hash__ = __hash__
    
    return cls