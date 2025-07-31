"""
JSON output formatting for analysis results.
"""

import json
from typing import Dict, Any
from pathlib import Path


class JSONExporter:
    """Exports analysis results to JSON format."""
    
    @staticmethod
    def export_analysis(analysis: Dict, output_path: str, pretty: bool = True) -> None:
        """
        Export analysis results to JSON file.
        
        Args:
            analysis: Analysis results dictionary
            output_path: Path to output JSON file
            pretty: Whether to format JSON with indentation
            
        Raises:
            IOError: If file cannot be written
        """
        output_file = Path(output_path)
        
        # Ensure directory exists
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Prepare data for JSON serialization
        json_data = JSONExporter._prepare_for_json(analysis)
        
        # Write to file
        with open(output_file, 'w') as f:
            if pretty:
                json.dump(json_data, f, indent=2, separators=(',', ': '))
            else:
                json.dump(json_data, f, separators=(',', ':'))
    
    @staticmethod
    def export_to_string(analysis: Dict, pretty: bool = True) -> str:
        """
        Convert analysis results to JSON string.
        
        Args:
            analysis: Analysis results dictionary
            pretty: Whether to format JSON with indentation
            
        Returns:
            JSON string representation
        """
        json_data = JSONExporter._prepare_for_json(analysis)
        
        if pretty:
            return json.dumps(json_data, indent=2, separators=(',', ': '))
        else:
            return json.dumps(json_data, separators=(',', ':'))
    
    @staticmethod
    def _prepare_for_json(data: Any) -> Any:
        """
        Prepare data for JSON serialization by handling special types.
        
        Args:
            data: Data to prepare
            
        Returns:
            JSON-serializable data
        """
        if isinstance(data, dict):
            return {key: JSONExporter._prepare_for_json(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [JSONExporter._prepare_for_json(item) for item in data]
        elif isinstance(data, Path):
            return str(data)
        elif hasattr(data, '__dict__'):
            # Handle custom objects by converting to dict
            return JSONExporter._prepare_for_json(data.__dict__)
        else:
            return data


class StructuredExporter:
    """Exports analysis results in various structured formats."""
    
    @staticmethod
    def export_csv_positions(positions: list, output_path: str) -> None:
        """
        Export position records to CSV format.
        
        Args:
            positions: List of position records
            output_path: Path to output CSV file
        """
        import csv
        
        output_file = Path(output_path)
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_file, 'w', newline='') as f:
            if not positions:
                f.write("index,timestamp,x,y,z,distance_from_origin\n")
                return
            
            writer = csv.writer(f)
            writer.writerow(['index', 'timestamp', 'x', 'y', 'z', 'distance_from_origin'])
            
            for pos in positions:
                writer.writerow([
                    pos['index'],
                    pos['timestamp'],
                    pos['position']['x'],
                    pos['position']['y'],
                    pos['position']['z'],
                    pos['distance_from_origin']
                ])
    
    @staticmethod
    def export_summary_yaml(analysis: Dict, output_path: str) -> None:
        """
        Export analysis summary to YAML format.
        
        Args:
            analysis: Analysis results dictionary
            output_path: Path to output YAML file
        """
        try:
            import yaml
        except ImportError:
            raise ImportError("PyYAML package required for YAML export")
        
        output_file = Path(output_path)
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Create summary data
        summary = {
            'file_info': {
                'path': analysis['file_path'],
                'size_bytes': analysis['file_size']
            },
            'header': analysis['header'],
            'relative_attitude': analysis['relative_attitude'],
            'statistics': analysis.get('statistics'),
            'record_count': len(analysis['position_records'])
        }
        
        with open(output_file, 'w') as f:
            yaml.dump(summary, f, default_flow_style=False, indent=2)
