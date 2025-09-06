#!/usr/bin/env python3
"""
Test script to demonstrate altitude error statistics output.
Creates sample data to show the statistics formatting.
"""

import pandas as pd
import numpy as np
from pathlib import Path
import tempfile
from distance_error_analysis import DistanceErrorAnalyzer

def create_sample_data():
    """Create sample data for testing the altitude statistics."""
    np.random.seed(42)  # For reproducible results
    
    # Create sample data with high resolutions (1024, 2048, 4096)
    resolutions = [512, 1024, 2048, 4096]  # Include 512 to test filtering
    fov_values = [10, 15, 20, 25, 30]
    
    data = []
    earth_radius = 6371000
    
    for resolution in resolutions:
        for fov in fov_values:
            # Generate 50 samples for each combination
            for i in range(50):
                # Altitude ranges from 200km to 800km
                altitude = np.random.uniform(200000, 800000)
                true_distance = earth_radius + altitude
                
                # Error scales with altitude and has some FOV/resolution dependence
                base_error = altitude * 0.001  # 0.1% of altitude as base
                fov_factor = 1 + (fov - 20) * 0.02  # FOV effect
                res_factor = 1 / np.sqrt(resolution / 1024)  # Resolution effect
                
                error = base_error * fov_factor * res_factor * (1 + np.random.normal(0, 0.3))
                measured_distance = true_distance + error
                relative_error = error / true_distance
                
                # Calculate focal length from FOV (reverse engineer)
                pixel_size = 5e-6  # 5 microns
                sensor_width = pixel_size * resolution
                focal_length = sensor_width / (2 * np.tan(np.radians(fov) / 2))
                
                data.append({
                    'image_file': f'test_combo{i}_angle0_res{resolution}_fov{fov}.png',
                    'true_distance': true_distance,
                    'measured_distance': measured_distance,
                    'error': error,
                    'relative_error': relative_error,
                    'nadir_deviation': np.random.uniform(0, 0.1),
                    'focal_length': focal_length,
                    'pixel_size': pixel_size,
                    'resolution': resolution,
                    'num_pixels': resolution * resolution,
                    'orientation': f"{np.random.uniform(-0.1, 0.1):.6f} {np.random.uniform(-0.1, 0.1):.6f} {np.random.uniform(-0.1, 0.1):.6f}"
                })
    
    return pd.DataFrame(data)

def main():
    """Create sample data and run the altitude statistics analysis."""
    print("Creating sample data for altitude error statistics demonstration...")
    
    # Create sample data
    df = create_sample_data()
    
    # Save to temporary CSV file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False) as f:
        df.to_csv(f.name, index=False)
        temp_csv_path = f.name
    
    try:
        print(f"Created sample dataset with {len(df)} records")
        print("Running altitude error statistics analysis...\n")
        
        # Initialize analyzer with sample data
        analyzer = DistanceErrorAnalyzer(temp_csv_path)
        
        # Run the altitude statistics
        analyzer.print_altitude_error_statistics()
        
    finally:
        # Clean up temporary file
        Path(temp_csv_path).unlink()

if __name__ == "__main__":
    main()
