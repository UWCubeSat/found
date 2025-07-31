#!/usr/bin/env python3
"""
Simple integration test for the FOUND File Analyzer tool.
"""

import tempfile
import struct
import zlib
from pathlib import Path
import sys
import os

# Add the tools directory to the path so we can import the analyzer
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

from analyzer.src.core import FoundFileAnalyzer


def create_test_found_file(num_positions=2):
    """Create a test .found file with valid CRC."""
    with tempfile.NamedTemporaryFile(suffix='.found', delete=False) as f:
        # Header
        magic = b'FOUN'
        version = 1
        num_pos = num_positions
        
        # Write header without CRC first
        header_without_crc = struct.pack('>4sII', magic, version, num_pos)
        
        # Calculate CRC
        crc = zlib.crc32(header_without_crc) & 0xffffffff
        
        # Write complete header
        f.write(header_without_crc)
        f.write(struct.pack('>I', crc))
        
        # Write quaternion (relative attitude)
        quaternion = (0.965926, 0.0, 0.258819, 0.0)
        for component in quaternion:
            f.write(struct.pack('>d', component))
        
        # Write position records
        for i in range(num_positions):
            timestamp = 1000000 + i * 100000
            x, y, z = 100.0 + i, 200.0 + i, 300.0 + i
            f.write(struct.pack('>Qddd', timestamp, x, y, z))
        
        return f.name


def test_basic_analysis():
    """Test basic file analysis."""
    print("Testing basic analysis...")
    
    test_file = create_test_found_file(2)
    try:
        analyzer = FoundFileAnalyzer(test_file)
        analysis = analyzer.analyze_file()
        
        # Check header
        assert analysis['header']['magic'] == 'FOUN'
        assert analysis['header']['version'] == 1
        assert analysis['header']['num_positions'] == 2
        assert analysis['header']['crc_valid'] == True
        
        # Check quaternion
        quat = analysis['relative_attitude']
        assert abs(quat['real'] - 0.965926) < 1e-6
        assert abs(quat['magnitude'] - 1.0) < 1e-6
        
        # Check positions
        assert len(analysis['position_records']) == 2
        assert analysis['position_records'][0]['timestamp'] == 1000000
        assert analysis['position_records'][1]['timestamp'] == 1100000
        
        print("✅ Basic analysis test passed")
        
    finally:
        os.unlink(test_file)


def test_empty_file():
    """Test analysis of empty file (no position records)."""
    print("Testing empty file analysis...")
    
    test_file = create_test_found_file(0)
    try:
        analyzer = FoundFileAnalyzer(test_file)
        analysis = analyzer.analyze_file()
        
        assert analysis['header']['num_positions'] == 0
        assert len(analysis['position_records']) == 0
        assert analysis['statistics'] is None
        
        print("✅ Empty file test passed")
        
    finally:
        os.unlink(test_file)


def test_invalid_file():
    """Test handling of invalid files."""
    print("Testing invalid file handling...")
    
    with tempfile.NamedTemporaryFile(suffix='.found', delete=False) as f:
        f.write(b'INVALID_CONTENT')
        invalid_file = f.name
    
    try:
        analyzer = FoundFileAnalyzer(invalid_file)
        try:
            analysis = analyzer.analyze_file()
            assert False, "Should have raised ValueError"
        except ValueError as e:
            # Accept either error message (could be too short or invalid magic)
            assert ("Invalid magic number" in str(e) or "header too short" in str(e))
        
        print("✅ Invalid file test passed")
        
    finally:
        os.unlink(invalid_file)


if __name__ == '__main__':
    print("Running FOUND File Analyzer tests...")
    print()
    
    test_basic_analysis()
    test_empty_file()
    test_invalid_file()
    
    print()
    print("🎉 All tests passed!")
