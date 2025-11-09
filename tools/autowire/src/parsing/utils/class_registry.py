"""Class registry for inheritance analysis."""

from typing import Dict, List, Set
from ..parsing.constructs import Class


class ClassRegistry:
    """Registry for managing class inheritance relationships."""
    
    def __init__(self):
        """Initialize empty registry."""
        self.classes: Dict[str, Class] = {}  # name -> Class construct
        self.inheritance_map: Dict[str, Set[str]] = {}  # base_class -> {subclass1, subclass2}
    
    def register_class(self, class_construct: Class) -> None:
        """Register a class and build inheritance relationships.
        
        Args:
            class_construct (Class): Class to register in the inheritance hierarchy
            
        Preconditions:
            class_construct must have a valid name
            All base classes should be registered before subclasses
        """
        self.classes[class_construct.name] = class_construct
        
        # Build inheritance map using base class names
        for access_level, base_class in class_construct.base_classes:
            base_name = base_class.name
            if base_name not in self.inheritance_map:
                self.inheritance_map[base_name] = set()
            self.inheritance_map[base_name].add(class_construct.name)
            
            # Also add to base class's subclasses list
            base_class.add_subclass(class_construct)
    
    def get_subclasses(self, class_name: str) -> List[Class]:
        """Get all direct subclasses of a given class.
        
        Args:
            class_name (str): Name of the base class
            
        Returns:
            List[Class]: List of direct subclass constructs
        """
        subclass_names = self.inheritance_map.get(class_name, set())
        return [self.classes[name] for name in subclass_names if name in self.classes]
    
    def get_all_subclasses(self, class_name: str) -> List[Class]:
        """Get all subclasses (direct and indirect) of a given class.
        
        Args:
            class_name (str): Name of the base class
            
        Returns:
            List[Class]: List of all subclass constructs (direct and indirect)
            
        Preconditions:
            Class hierarchy must be acyclic (no circular inheritance)
        """
        result = []
        visited = set()
        
        def collect_subclasses(current_class: str):
            if current_class in visited:
                return
            visited.add(current_class)
            
            direct_subclasses = self.inheritance_map.get(current_class, set())
            for subclass in direct_subclasses:
                if subclass in self.classes:
                    result.append(self.classes[subclass])
                    collect_subclasses(subclass)  # Recursive for indirect subclasses
        
        collect_subclasses(class_name)
        return result
    
    def get_concrete_subclasses(self, class_name: str) -> List[Class]:
        """Get all concrete (non-abstract) subclasses of a given class.
        
        Args:
            class_name (str): Name of the base class
            
        Returns:
            List[Class]: List of concrete subclass constructs that can be instantiated
            
        Preconditions:
            Class hierarchy must be acyclic (no circular inheritance)
        """
        all_subclasses = self.get_all_subclasses(class_name)
        return [cls for cls in all_subclasses if cls.can_be_instantiated()]
    
    def get_class(self, name: str) -> Class:
        """Get class construct by name.
        
        Args:
            name (str): Class name to look up
            
        Returns:
            Class: Class construct, or None if not found
        """
        return self.classes.get(name)