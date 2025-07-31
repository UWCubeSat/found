#!/usr/bin/env python3
"""
Example usage of the FOUND File Analyzer tool.

This script demonstrates how to use the analyzer both programmatically
and via command line interface.
"""

import tempfile
import struct
import zlib
import subprocess
import sys
from pathlib import Path

# Add the analyzer to the path
sys.path.insert(0, str(Path(__file__).parent))
from src.core import FoundFileAnalyzer


def create_example_found_file():
    """Create an example .found file for demonstration."""
    with tempfile.NamedTemporaryFile(suffix='.found', delete=False) as f:
        # Header
        magic = b'FOUN'
        version = 1
        num_pos = 2
        
        # Calculate CRC for proper header
        header_without_crc = struct.pack('>4sII', magic, version, num_pos)
        crc = zlib.crc32(header_without_crc) & 0xffffffff
        
        # Write complete header
        f.write(header_without_crc)
        f.write(struct.pack('>I', crc))
        
        # Write quaternion (relative attitude) - represents a 45-degree rotation
        quaternion = (0.9238795, 0.0, 0.3826834, 0.0)
        for component in quaternion:
            f.write(struct.pack('>d', component))
        
        # Write position records
        positions = [
            (1609459200000000, 6371000.0, 0.0, 1000.0),      # New Year 2021, 1km altitude
            (1609459260000000, 6371000.0, 100.0, 1005.0),    # 1 minute later, slightly moved
        ]
        for timestamp, x, y, z in positions:
            f.write(struct.pack('>Qddd', timestamp, x, y, z))
        
        return f.name


def example_programmatic_usage():
    """Example of using the analyzer programmatically."""
    print("=== Programmatic Usage Example ===")
    
    # Create test file
    test_file = create_example_found_file()
    
    try:
        # Create analyzer instance
        analyzer = FoundFileAnalyzer(test_file)
        
        # Perform analysis
        analysis = analyzer.analyze_file()
        
        # Access specific data
        print(f"File: {analysis['file_path']}")
        print(f"File format version: {analysis['header']['version']}")
        print(f"Number of positions: {analysis['header']['num_positions']}")
        print(f"File integrity: {'Valid' if analysis['header']['crc_valid'] else 'Invalid'}")
        
        # Access quaternion data
        quat = analysis['relative_attitude']
        print(f"Relative attitude magnitude: {quat['magnitude']:.6f}")
        
        # Access position data
        if analysis['position_records']:
            first_pos = analysis['position_records'][0]
            print(f"First position timestamp: {first_pos['timestamp']}")
            print(f"First position coordinates: ({first_pos['position']['x']:.1f}, "
                  f"{first_pos['position']['y']:.1f}, {first_pos['position']['z']:.1f})")
        
        print()
        
    finally:
        # Clean up
        Path(test_file).unlink()


def example_command_line_usage():
    """Example of using the analyzer via command line."""
    print("=== Command Line Usage Example ===")
    
    # Create test file
    test_file = create_example_found_file()
    
    try:
        print("Running: python -m tools.analyzer <file>")
        result = subprocess.run([
            sys.executable, '-m', 'tools.analyzer', test_file
        ], capture_output=True, text=True, cwd=Path(__file__).parent.parent.parent)
        
        print("Output:")
        print(result.stdout)
        
        if result.stderr:
            print("Errors:")
            print(result.stderr)
            
    finally:
        # Clean up
        Path(test_file).unlink()


if __name__ == '__main__':
    print("FOUND File Analyzer - Usage Examples")
    print("=" * 50)
    print()
    
    example_programmatic_usage()
    example_command_line_usage()
    
    print("For more information, see the README.md file or run:")
    print("python -m tools.analyzer --help")
