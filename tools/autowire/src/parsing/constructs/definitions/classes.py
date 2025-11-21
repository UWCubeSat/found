"""Class-related constructs."""

from dataclasses import dataclass
from typing import List, Optional, Tuple, Union
from ..core.base import Construct, Definition
from .functions import Function, Parameter
from ..statements.variables import Variable
from ..types.types import Type
from .misc import Comment
from ....common.annotations import equals_hash


@dataclass
class FriendDeclaration(Construct):
    """Friend class or function declaration."""
    target: Union['Class', Function]


@dataclass
class Method(Function):
    """Class methods with method-specific attributes."""
    is_virtual: bool = False
    is_const: bool = False
    is_pure_virtual: bool = False


@dataclass
class Constructor(Method):
    """Class constructors with parameter lists."""
    pass


@dataclass
class Destructor(Method):
    """Class destructors (parsed but not used for dependency injection)."""
    
    def __post_init__(self):
        """Validate destructor name after initialization."""
        if not self.name.startswith('~'):
            raise ValueError(f"Destructor {self.name} must have ~ in front")


@equals_hash
class AccessSection(Construct):
    """Container for class members with specific access level."""
    
    def __init__(self,
                 constructors: List[Constructor] = None,
                 destructors: List[Destructor] = None,
                 methods: List[Tuple[Optional['Class'], Method]] = None,
                 members: List[Variable] = None,
                 nested_classes: List['Class'] = None,
                 friend_declarations: List[FriendDeclaration] = None):
        """Initialize access section."""
        super().__init__()
        self.constructors: List[Constructor] = constructors or []
        self.destructors: List[Destructor] = destructors or []
        self.methods: List[Tuple[Optional[Class], Method]] = methods or []  # (inherited_from_class, method)
        self.members: List[Variable] = members or []
        self.nested_classes: List[Class] = nested_classes or []
        self.friend_declarations: List[FriendDeclaration] = friend_declarations or []            
        
        for constructor in self.constructors:
            if constructor.parent != self.parent:
                raise ValueError("All constructors must have their class as the parent")
        for destructor in self.destructors:
            if destructor.parent != self.parent:
                raise ValueError("All destructors must have their class as the parent")
        for _, method in self.methods:
            if method.parent != self.parent:
                raise ValueError("All methods must have their class as the parent")
        for member in self.members:
            if member.parent != self.parent:
                raise ValueError("All members must have their class as the parent")
        for nested_class in self.nested_classes:
            if nested_class.parent != self.parent:
                raise ValueError("All nested classes must have their class as the parent")
        for friend_decl in self.friend_declarations:
            if friend_decl.parent != self.parent:
                raise ValueError("All friend declarations must have their class as the parent")
    
    def add_nested_class(self, nested_class: 'Class') -> None:
        """Add a nested class to this access section.
        
        Args:
            nested_class (Class): Nested class to add
        """
        nested_class.set_parent(self.parent)
        self.nested_classes.append(nested_class)
    
    def add_friend_declaration(self, friend_decl: FriendDeclaration) -> None:
        """Add a friend declaration to this access section.
        
        Args:
            friend_decl (FriendDeclaration): Friend declaration to add
        """
        friend_decl.set_parent(self.parent)
        self.friend_declarations.append(friend_decl)


@equals_hash
class Class(Definition):
    """Class and struct definitions with inheritance, constructors, methods, access specifiers."""
    
    def __init__(self, name: str, is_struct: bool = False):
        """Initialize class construct with minimal information.
        
        Args:
            name (str): Class name
            is_struct (bool): True if this is a struct, False for class
        """
        super().__init__(name)
        self.is_struct = is_struct
        self.base_classes: List[Tuple[str, Class]] = []
        self.template_parameters: List[str] = []
        self.can_brace_initialize = False
        self.brace_init_members: List[Variable] = []
        self.comment: Optional[Comment] = None
        self.namespace: Optional[str] = None
        self.subclasses: List[Class] = []
        
        # Create access sections with this class as parent
        self.public = AccessSection()
        self.private = AccessSection()
        self.protected = AccessSection()
        self.public.set_parent(self)
        self.private.set_parent(self)
        self.protected.set_parent(self)
    
    def add_base_class(self, access: str, base_class: 'Class') -> None:
        """Add a base class and update inheritance relationships.
        
        Args:
            access (str): Inheritance access level ('public', 'private', 'protected')
            base_class (Class): Base class to inherit from
        """
        self.base_classes.append((access, base_class))
        base_class.subclasses.append(self)
        self._inherit_methods_from_base(access, base_class)
    
    def add_subclass(self, subclass: 'Class') -> None:
        """Add a direct subclass to this class's subclass list.
        
        Args:
            subclass (Class): Class that inherits from this class
        """
        if subclass not in self.subclasses:
            self.subclasses.append(subclass)
    
    def get_direct_subclasses(self) -> List['Class']:
        """Get all direct subclasses of this class.
        
        Returns:
            List[Class]: Copy of direct subclasses list
        """
        return self.subclasses.copy()
    
    def get_all_subclasses(self) -> List['Class']:
        """Get all subclasses (direct and indirect) of this class.
        
        Returns:
            List[Class]: List of all subclasses in breadth-first order
            
        Preconditions:
            Class hierarchy must be acyclic (no circular inheritance)
        """
        result = []
        visited = set()
        queue = [self]
        
        while queue:
            current_class = queue.pop(0)
            if current_class in visited:
                continue
            visited.add(current_class)
            
            for subclass in current_class.subclasses:
                result.append(subclass)
                queue.append(subclass)
        
        return result
    
    def get_concrete_subclasses(self) -> List['Class']:
        """Get all concrete (non-abstract) subclasses of this class.
        
        Returns:
            List[Class]: List of all concrete subclasses that can be instantiated
            
        Preconditions:
            Class hierarchy must be acyclic (no circular inheritance)
        """
        return [cls for cls in self.get_all_subclasses() if cls.can_be_instantiated()]
    
    def _inherit_methods_from_base(self, inheritance_access: str, base_class: 'Class') -> None:
        """Inherit methods from a single base class.
        
        Args:
            inheritance_access (str): Access level of inheritance
            base_class (Class): Base class to inherit from
        """
        existing_method_names = {func.name for _, func in self.public.methods + self.protected.methods + self.private.methods}
        
        # Only inherit public and protected methods
        for inherited_from, method in base_class.public.methods + base_class.protected.methods:
            if method.name not in existing_method_names:
                # Create inherited method copy
                inherited_method = Method(
                    name=method.name,
                    return_type=method.return_type,
                    parameters=method.parameters,
                    template_parameters=method.template_parameters,
                    is_virtual=method.is_virtual,
                    is_static=method.is_static,
                    is_const=method.is_const,
                    is_pure_virtual=method.is_pure_virtual,
                    comment=method.comment,
                    namespace=method.namespace
                )
                inherited_method.set_parent(self)
                
                # Determine final access level
                final_access = self._determine_inherited_access(inheritance_access, method, base_class)
                original_source = inherited_from or base_class
                
                # Add to appropriate access section
                if final_access == "public":
                    self.public.methods.append((original_source, inherited_method))
                elif final_access == "protected":
                    self.protected.methods.append((original_source, inherited_method))
                elif final_access == "private":
                    self.private.methods.append((original_source, inherited_method))
    
    def _determine_inherited_access(self, inheritance_access: str, method: Method, base_class: 'Class') -> str:
        """Determine the access level of an inherited method.
        
        Args:
            inheritance_access (str): Access level of inheritance ('public', 'private', 'protected')
            method (Function): Method being inherited
            base_class (Class): Class the method is inherited from
            
        Returns:
            str: Final access level for the inherited method
        
        Raises:
            ValueError if method is private in base class
            
        Preconditions:
            inheritance_access must be 'public', 'private', or 'protected'
            method must exist in base_class's public or protected sections
        """
        # Determine original access level of the method in base class
        if any(func == method for _, func in base_class.public.methods):
            original_access = "public"
        elif any(func == method for _, func in base_class.protected.methods):
            original_access = "protected"
        else:
            # Private methods in base class are never inherited
            raise ValueError("Private methods should not be inherited")
        
        # Apply C++ inheritance access rules
        if inheritance_access == "private":
            return "private"  # All inherited public/protected members become private (but still accessible internally)
        elif inheritance_access == "protected":
            return "protected"  # All inherited public/protected members become protected
        else:  # public inheritance
            return original_access  # Access level preserved (public stays public, protected stays protected)
    
    def has_pure_virtual_methods(self) -> bool:
        """Check if class has any pure virtual methods (includes inherited ones).
        
        Returns:
            bool: True if any method is pure virtual, False otherwise
        """
        for _, method in self.public.methods + self.private.methods + self.protected.methods:
            if method.is_pure_virtual:
                return True
        return False
    
    @property
    def is_pure_virtual(self) -> bool:
        """Check if class is pure virtual (derived from having pure virtual methods).
        
        Returns:
            bool: True if class has any pure virtual methods
        """
        return self.has_pure_virtual_methods()
    
    def is_abstract(self) -> bool:
        """Check if class is abstract (has pure virtual methods).
        
        Returns:
            bool: True if class cannot be instantiated due to pure virtual methods
        """
        return self.is_pure_virtual
    
    def can_be_instantiated(self) -> bool:
        """Check if class can be directly instantiated (not abstract).
        
        Returns:
            bool: True if class has no pure virtual methods and can be instantiated
        """
        return not self.is_pure_virtual
    
    def add_access_section(self, access_level: str, section: AccessSection) -> None:
        """Add or replace an access section.
        
        Args:
            access_level (str): Access level ('public', 'private', 'protected')
            section (AccessSection): Access section to add
        """
        section.set_parent(self)
        if access_level == "public":
            self.public = section
        elif access_level == "private":
            self.private = section
        elif access_level == "protected":
            self.protected = section
        else:
            raise ValueError(f"Invalid access level: {access_level}")