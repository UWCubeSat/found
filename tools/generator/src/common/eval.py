def float_equals(num1: float, num2: float, tolerance: float = 1e-3) -> bool:
    """See's if a float is equal to another float

    Args:
        num1 (float): One float to evaluate
        num2 (float): The other float to evaluate against
        tolerance (float, optional): The maximum difference. Defaults to 1e-3.

    Returns:
        bool: True iff num1 and num2 are roughly the same (their difference is less than tolerance)
    """
    return abs(num1 - num2) < tolerance
