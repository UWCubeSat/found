"""Class-related constructs."""

from typing import List, Optional, Tuple
from ..core.base import Construct, Definition
from .functions import Function, Parameter
from ..statements.variables import Variable
from .misc import Comment
from ....common.annotations import equals_hash


@equals_hash
class Constructor(Function):
    """Class constructors with parameter lists."""
    
    def __init__(self, name: str, parameters: List[Parameter] = None):
        """Initialize constructor.
        
        Args:
            name (str): The name of the constructor
            parameters (List[Parameter], optional): List of constructor parameters, defaults to empty list
        """
        super().__init__(name, None, parameters=parameters)
        self.parameters = parameters or []


@equals_hash
class Destructor(Function):
    """Class destructors (parsed but not used for dependency injection)."""
    
    def __init__(self, name: str, is_virtual: bool = False):
        """Initialize destructor.
        
        Args:
            name (str): The name of the destructor
            is_virtual (bool): Whether destructor is virtual
        """
        if not name[0] == '~':
            raise ValueError(f"Destructor {name} must have ~ in front")
        
        super().__init__(name, None)
        self.is_virtual = is_virtual


@equals_hash
class AccessSection(Construct):
    """Container for class members with specific access level."""
    
    def __init__(self,
                 constructors: List[Constructor] = None,
                 destructors: List[Destructor] = None,
                 methods: List[Tuple[Optional['Class'], Function]] = None,
                 members: List[Variable] = None):
        """Initialize access section."""
        super().__init__()
        self.constructors: List[Constructor] = constructors or []
        self.destructors: List[Destructor] = destructors or []
        self.methods: List[Tuple[Optional[Class], Function]] = methods or []  # (inherited_from_class, function)
        self.members: List[Variable] = members or []            
        
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


@equals_hash
class Class(Definition):
    """Class and struct definitions with inheritance, constructors, methods, access specifiers.
    
    Note: Access sections passed to the constructor should contain only methods defined directly
    in this class. Inherited methods are automatically added by the class during construction.
    """
    
    def __init__(self, name: str, is_struct: bool = False, base_classes: List[Tuple[str, 'Class']] = None,
                 template_parameters: List[str] = None, can_brace_initialize: bool = False,
                 brace_init_members: List[Variable] = None, comment: Optional[Comment] = None,
                 namespace: Optional[str] = None,
                 public: Optional[AccessSection] = None, private: Optional[AccessSection] = None,
                 protected: Optional[AccessSection] = None):
        """Initialize class construct.
        
        Args:
            name (str): Class name
            is_struct (bool): True if this is a struct, False for class
            base_classes (List[Tuple[str, Class]], optional): List of (access_level, Class) tuples for inheritance
            template_parameters (List[str], optional): List of template parameter names
            can_brace_initialize (bool): True if class supports brace initialization
            brace_init_members (List[Variable], optional): List of members for brace initialization
            comment (Comment, optional): Associated comment block
            namespace (str, optional): Namespace containing this class
            public (AccessSection, optional): Public access section with directly defined members
            private (AccessSection, optional): Private access section with directly defined members
            protected (AccessSection, optional): Protected access section with directly defined members
            
        Preconditions:
            name must be non-empty string
            base_classes access levels must be 'public', 'private', or 'protected'
            Access sections should contain only directly defined methods
            All base classes must be fully constructed
        """
        super().__init__(name)
        self.is_struct = is_struct
        self.base_classes: List[Tuple[str, Class]] = base_classes or []  # List of (access_level, Class) tuples
        self.template_parameters = template_parameters or []
        self.can_brace_initialize = can_brace_initialize
        self.brace_init_members = brace_init_members or []
        self.comment = comment
        self.namespace = namespace
        self.subclasses: List[Class] = []  # List of Class references that inherit from this class
        
        # Access level buckets - inject or create, then set parent
        self.public = public or AccessSection()
        self.private = private or AccessSection()
        self.protected = protected or AccessSection()
        
        # Set this class as parent for all access sections
        self.public.parent = self
        self.private.parent = self
        self.protected.parent = self
        
        # Inherit methods from base classes (in inheritance order)
        self._inherit_methods_from_bases()
    
    def add_subclass(self, subclass: 'Class') -> None:
        """Add a direct subclass to this class's subclass list.
        
        Args:
            subclass (Class): Class that inherits from this class
            
        Preconditions:
            This class must be a direct base class of the subclass
        """
        # Only add if this class is a direct base class of the subclass
        base_classes = [base_class for _, base_class in subclass.base_classes]
        if self in base_classes and subclass not in self.subclasses:
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
    
    def _inherit_methods_from_bases(self) -> None:
        """Inherit methods from base classes in inheritance order.
        
        Preconditions:
            All base classes must be fully constructed
            Access sections must contain only directly defined methods
            Base classes must not contain circular inheritance
        """
        existing_method_names = {func.name for _, func in self.public.methods + self.protected.methods + self.private.methods}
        for inheritance_access, base_class in self.base_classes:
            # Only inherit public and protected methods (private methods in base are never inherited)
            for inherited_from, method in base_class.public.methods + base_class.protected.methods:
                # Check if method is already overridden in this class                
                if method.name not in existing_method_names:
                    # Create inherited method copy
                    inherited_method = Function(
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
                    inherited_method.parent = self
                    
                    # Determine final access level based on inheritance access and original access
                    final_access = self._determine_inherited_access(inheritance_access, method, base_class)
                    
                    # Track original source class (if method was already inherited, keep original source)
                    original_source = inherited_from or base_class
                    
                    # Add to appropriate access section with inheritance tracking
                    if final_access == "public":
                        self.public.methods.append((original_source, inherited_method))
                    elif final_access == "protected":
                        self.protected.methods.append((original_source, inherited_method))
                    elif final_access == "private":
                        self.private.methods.append((original_source, inherited_method))  # Still accessible internally
    
    def _determine_inherited_access(self, inheritance_access: str, method: Function, base_class: 'Class') -> str:
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