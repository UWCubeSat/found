"""
File format validation utilities for .found files.
"""

from typing import Dict, Optional


class FormatValidator:
    """Validates .found file format and data integrity."""
    
    @staticmethod
    def validate_header(header: Dict) -> Dict[str, bool]:
        """
        Validate header information.
        
        Args:
            header: Header dictionary from analyzer
            
        Returns:
            Dictionary with validation results
        """
        validations = {
            'magic_valid': header['magic'] == 'FOUN',
            'version_supported': header['version'] == 1,
            'crc_valid': header['crc_valid'],
            'position_count_reasonable': 0 <= header['num_positions'] <= 1000000
        }
        
        validations['header_valid'] = all(validations.values())
        return validations
    
    @staticmethod
    def validate_quaternion(quaternion: Dict) -> Dict[str, bool]:
        """
        Validate quaternion data.
        
        Args:
            quaternion: Quaternion dictionary from analyzer
            
        Returns:
            Dictionary with validation results
        """
        validations = {
            'is_unit': quaternion['is_unit'],
            'no_nan_values': all(
                not (val != val)  # Check for NaN
                for val in [quaternion['real'], quaternion['i'], quaternion['j'], quaternion['k']]
            ),
            'reasonable_magnitude': 0.9 <= quaternion['magnitude'] <= 1.1
        }
        
        validations['quaternion_valid'] = all(validations.values())
        return validations
    
    @staticmethod
    def validate_positions(position_records: list, statistics: Optional[Dict] = None) -> Dict[str, bool]:
        """
        Validate position records.
        
        Args:
            position_records: List of position records
            statistics: Optional statistics dictionary
            
        Returns:
            Dictionary with validation results
        """
        if not position_records:
            return {
                'positions_valid': True,
                'no_positions': True
            }
        
        validations = {
            'no_positions': False,
            'timestamps_ascending': True,
            'positions_reasonable': True,
            'no_nan_positions': True
        }
        
        # Check timestamp ordering
        for i in range(1, len(position_records)):
            if position_records[i]['timestamp'] <= position_records[i-1]['timestamp']:
                validations['timestamps_ascending'] = False
                break
        
        # Check for reasonable positions and NaN values
        for record in position_records:
            pos = record['position']
            # Check for NaN
            if any(val != val for val in [pos['x'], pos['y'], pos['z']]):
                validations['no_nan_positions'] = False
            
            # Check for reasonable distances (within solar system)
            if record['distance_from_origin'] > 1e12:  # 1 trillion meters
                validations['positions_reasonable'] = False
        
        validations['positions_valid'] = all(validations.values())
        return validations


class IntegrityChecker:
    """Checks overall file integrity and consistency."""
    
    @staticmethod
    def check_file_integrity(analysis: Dict) -> Dict[str, any]:
        """
        Perform comprehensive integrity check.
        
        Args:
            analysis: Complete analysis dictionary
            
        Returns:
            Dictionary with integrity check results
        """
        validator = FormatValidator()
        
        # Individual component validations
        header_validation = validator.validate_header(analysis['header'])
        quaternion_validation = validator.validate_quaternion(analysis['relative_attitude'])
        position_validation = validator.validate_positions(
            analysis['position_records'], 
            analysis.get('statistics')
        )
        
        # File-level checks
        expected_positions = analysis['header']['num_positions']
        actual_positions = len(analysis['position_records'])
        
        file_checks = {
            'position_count_matches': expected_positions == actual_positions,
            'file_size_reasonable': analysis['file_size'] >= 48,  # Minimum size
            'all_positions_read': analysis['positions_read'] == expected_positions
        }
        
        # Overall integrity
        overall_valid = (
            header_validation['header_valid'] and
            quaternion_validation['quaternion_valid'] and
            position_validation['positions_valid'] and
            all(file_checks.values())
        )
        
        return {
            'overall_valid': overall_valid,
            'header_validation': header_validation,
            'quaternion_validation': quaternion_validation,
            'position_validation': position_validation,
            'file_checks': file_checks,
            'issues_found': not overall_valid
        }
