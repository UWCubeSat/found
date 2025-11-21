"""Auto-generate constructor for classes."""


def auto_init(**fields):
    """Decorator that auto-generates __init__ for specified fields.
    
    Usage:
        @auto_init(name=str, age=(int, 25), items=(list, None))
        class MyClass:
            pass
    
    Field format:
        - field_name=type for required fields
        - field_name=(type, default) for optional fields
    """
    def decorator(cls):
        field_names = list(fields.keys())
        
        def __init__(self, *args, **kwargs):
            # Handle positional arguments
            for i, arg in enumerate(args):
                if i < len(field_names):
                    kwargs[field_names[i]] = arg
            
            # Process all fields
            for field_name, field_spec in fields.items():
                if isinstance(field_spec, tuple):
                    field_type, default = field_spec
                    value = kwargs.get(field_name, default)
                else:
                    field_type = field_spec
                    if field_name not in kwargs:
                        raise TypeError(f"Missing required argument: {field_name}")
                    value = kwargs[field_name]
                setattr(self, field_name, value)
        
        cls.__init__ = __init__
        return cls
    return decorator