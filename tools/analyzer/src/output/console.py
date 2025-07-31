"""
Console output formatting for analysis results.
"""

from typing import Dict


class ConsoleFormatter:
    """Formats analysis results for console display."""
    
    @staticmethod
    def format_summary(analysis: Dict) -> str:
        """
        Format a comprehensive summary of the analysis.
        
        Args:
            analysis: Analysis results dictionary
            
        Returns:
            Formatted string for console output
        """
        lines = []
        
        # Header
        lines.append(f"=== FOUND File Analysis: {analysis['file_path']} ===")
        lines.append(f"File size: {analysis['file_size']} bytes")
        lines.append("")
        
        # Header information
        header = analysis['header']
        lines.append("📄 Header Information:")
        lines.append(f"  Magic: {header['magic']}")
        lines.append(f"  Version: {header['version']}")
        lines.append(f"  Number of positions: {header['num_positions']}")
        lines.append(f"  CRC: 0x{header['crc']:08X}")
        lines.append(f"  CRC Valid: {'✅' if header['crc_valid'] else '❌'}")
        if not header['crc_valid']:
            lines.append(f"  Expected CRC: 0x{header['calculated_crc']:08X}")
        lines.append("")
        
        # Relative attitude (quaternion)
        quat = analysis['relative_attitude']
        lines.append("🧭 Relative Attitude (Quaternion):")
        lines.append(f"  Real: {quat['real']:.6f}")
        lines.append(f"  i: {quat['i']:.6f}")
        lines.append(f"  j: {quat['j']:.6f}")
        lines.append(f"  k: {quat['k']:.6f}")
        lines.append(f"  Magnitude: {quat['magnitude']:.6f}")
        lines.append(f"  Unit quaternion: {'✅' if quat['is_unit'] else '❌'}")
        lines.append("")
        
        # Position records
        positions = analysis['position_records']
        lines.append(f"📍 Position Records: {len(positions)} found")
        if positions:
            lines.append("  Recent positions:")
            recent_count = min(5, len(positions))
            for pos in positions[-recent_count:]:
                lines.append(
                    f"    [{pos['index']}] t={pos['timestamp']}, "
                    f"pos=({pos['position']['x']:.3f}, {pos['position']['y']:.3f}, {pos['position']['z']:.3f}), "
                    f"dist={pos['distance_from_origin']:.3f}"
                )
        lines.append("")
        
        # Statistics
        stats = analysis.get('statistics')
        if stats:
            lines.append("📊 Statistics:")
            lines.append(f"  Distance range: {stats['min_distance']:.3f} to {stats['max_distance']:.3f}")
            lines.append(f"  Average distance: {stats['avg_distance']:.3f}")
            lines.append(f"  Time span: {stats['time_span']} microseconds")
            if stats['time_span'] > 0:
                lines.append(f"  Duration: {stats['duration_seconds']:.2f} seconds")
        
        return "\n".join(lines)
    
    @staticmethod
    def format_verbose_positions(positions: list) -> str:
        """
        Format all position records in verbose mode.
        
        Args:
            positions: List of position records
            
        Returns:
            Formatted string for verbose position output
        """
        if not positions:
            return "No position records found."
        
        lines = ["🔍 All Position Records:"]
        for pos in positions:
            lines.append(
                f"  [{pos['index']:3d}] timestamp={pos['timestamp']:12d}, "
                f"x={pos['position']['x']:8.3f}, y={pos['position']['y']:8.3f}, z={pos['position']['z']:8.3f}, "
                f"dist={pos['distance_from_origin']:8.3f}"
            )
        
        return "\n".join(lines)
    
    @staticmethod
    def format_integrity_check(file_path: str, header: Dict, passed: bool) -> str:
        """
        Format integrity check results.
        
        Args:
            file_path: Path to the file
            header: Header information
            passed: Whether the check passed
            
        Returns:
            Formatted string for integrity check output
        """
        if passed:
            return (
                f"✅ File integrity check passed: {file_path}\n"
                f"   Version: {header['version']}, Positions: {header['num_positions']}"
            )
        else:
            return (
                f"❌ File integrity check failed: {file_path}\n"
                f"   Expected CRC: 0x{header['calculated_crc']:08X}\n"
                f"   Found CRC: 0x{header['crc']:08X}"
            )
    
    @staticmethod
    def format_error(error_type: str, message: str) -> str:
        """
        Format error messages consistently.
        
        Args:
            error_type: Type of error (e.g., "Error", "File format error")
            message: Error message
            
        Returns:
            Formatted error string
        """
        return f"❌ {error_type}: {message}"
