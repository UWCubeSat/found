"""
Constants for .found file format.
"""

# File format constants
MAGIC_NUMBER = b'FOUN'
HEADER_SIZE = 16  # bytes: magic(4) + version(4) + num_positions(4) + crc(4)
QUATERNION_SIZE = 32  # bytes: 4 doubles * 8 bytes each
POSITION_RECORD_SIZE = 32  # bytes: timestamp(8) + position(3*8)

# Validation thresholds
UNIT_QUATERNION_TOLERANCE = 1e-6
CRC_FIELD_OFFSET = 12  # CRC is calculated on first 12 bytes of header

# Default file format version
DEFAULT_VERSION = 1
