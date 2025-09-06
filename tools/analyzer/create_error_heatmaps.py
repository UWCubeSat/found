#!/usr/bin/env python3
"""
Create large heatmaps showing FOV vs Resolution for mean error and standard deviation.
Includes all resolutions in the dataset.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

def create_error_heatmaps(csv_file_path):
    """Create comprehensive FOV vs Resolution heatmaps for error analysis."""
    
    # Read the data
    print(f"Reading data from {csv_file_path}")
    df = pd.read_csv(csv_file_path)
    
    # Calculate altitude and absolute error
    earth_radius = 6371000  # meters
    df['altitude'] = df['true_distance'] - earth_radius
    df['abs_error'] = abs(df['measured_distance'] - df['true_distance'])
    
    print(f"Total samples: {len(df)}")
    print(f"FOV range: {df['fov_rounded'].min():.1f}° to {df['fov_rounded'].max():.1f}°")
    print(f"Resolution range: {df['resolution'].min()} to {df['resolution'].max()}")
    print(f"Altitude range: {df['altitude'].min()/1000:.1f} km to {df['altitude'].max()/1000:.1f} km")
    
    # Use the unique fov_rounded values directly instead of binning
    fov_values = sorted(df['fov_rounded'].unique())
    resolution_bins = sorted(df['resolution'].unique())  # Use actual resolution values
    
    print(f"FOV values: {len(fov_values)} unique values: {fov_values}")
    print(f"Resolution values: {len(resolution_bins)} unique resolutions")
    
    # Group by FOV rounded value and resolution to calculate statistics
    grouped = df.groupby(['fov_rounded', 'resolution'])['abs_error'].agg(['mean', 'std', 'count']).reset_index()
    
    # Create pivot tables for heatmaps
    mean_pivot = grouped.pivot(index='fov_rounded', columns='resolution', values='mean')
    std_pivot = grouped.pivot(index='fov_rounded', columns='resolution', values='std')
    count_pivot = grouped.pivot(index='fov_rounded', columns='resolution', values='count')
    
    # Set up the figure with two large subplots
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(24, 10))
    
    # Define a common colormap
    cmap = 'viridis'
    
    # 1. Mean Error Heatmap
    im1 = sns.heatmap(mean_pivot, 
                      annot=True, 
                      fmt='.0f', 
                      cmap=cmap,
                      cbar_kws={'label': 'Mean Absolute Error (meters)'},
                      ax=ax1,
                      annot_kws={'size': 8})
    
    ax1.set_title('Mean Absolute Error: FOV vs Resolution\n(Higher values = worse performance)', 
                  fontsize=16, fontweight='bold', pad=20)
    ax1.set_xlabel('Camera Resolution (pixels)', fontsize=14, fontweight='bold')
    ax1.set_ylabel('Field of View (degrees)', fontsize=14, fontweight='bold')
    ax1.tick_params(axis='x', rotation=45, labelsize=10)
    ax1.tick_params(axis='y', labelsize=10)
    
    # 2. Standard Deviation Heatmap
    im2 = sns.heatmap(std_pivot, 
                      annot=True, 
                      fmt='.0f', 
                      cmap='plasma',
                      cbar_kws={'label': 'Error Standard Deviation (meters)'},
                      ax=ax2,
                      annot_kws={'size': 8})
    
    ax2.set_title('Error Standard Deviation: FOV vs Resolution\n(Higher values = more variable performance)', 
                  fontsize=16, fontweight='bold', pad=20)
    ax2.set_xlabel('Camera Resolution (pixels)', fontsize=14, fontweight='bold')
    ax2.set_ylabel('Field of View (degrees)', fontsize=14, fontweight='bold')
    ax2.tick_params(axis='x', rotation=45, labelsize=10)
    ax2.tick_params(axis='y', labelsize=10)
    
    # Add overall statistics as text
    overall_mean = df['abs_error'].mean()
    overall_std = df['abs_error'].std()
    total_samples = len(df)
    
    fig.suptitle(f'Satellite Distance Error Analysis: FOV vs Resolution Heatmaps\n' +
                f'Total Samples: {total_samples:,} | Overall Mean Error: {overall_mean:.0f}m | Overall Std: {overall_std:.0f}m',
                fontsize=18, fontweight='bold', y=0.98)
    
    plt.tight_layout()
    
    # Save the plot
    output_dir = Path(csv_file_path).parent
    output_file = output_dir / 'fov_resolution_error_heatmaps.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Large heatmaps saved to: {output_file}")
    
    # Print summary statistics
    print("\n" + "="*80)
    print("HEATMAP SUMMARY STATISTICS")
    print("="*80)
    
    print(f"\nMean Error Heatmap Statistics:")
    print(f"  Minimum mean error: {mean_pivot.min().min():.0f}m")
    print(f"  Maximum mean error: {mean_pivot.max().max():.0f}m")
    print(f"  Range: {mean_pivot.max().max() - mean_pivot.min().min():.0f}m")
    
    print(f"\nStandard Deviation Heatmap Statistics:")
    print(f"  Minimum std deviation: {std_pivot.min().min():.0f}m")
    print(f"  Maximum std deviation: {std_pivot.max().max():.0f}m")
    print(f"  Range: {std_pivot.max().max() - std_pivot.min().min():.0f}m")
    
    # Find best and worst combinations
    mean_flat = mean_pivot.stack().reset_index()
    mean_flat.columns = ['fov', 'resolution', 'mean_error']
    mean_flat = mean_flat.dropna()
    
    best_combo = mean_flat.loc[mean_flat['mean_error'].idxmin()]
    worst_combo = mean_flat.loc[mean_flat['mean_error'].idxmax()]
    
    print(f"\nBest Parameter Combination (Lowest Mean Error):")
    print(f"  FOV: {best_combo['fov']:.1f}°, Resolution: {best_combo['resolution']}, Mean Error: {best_combo['mean_error']:.0f}m")
    
    print(f"\nWorst Parameter Combination (Highest Mean Error):")
    print(f"  FOV: {worst_combo['fov']:.1f}°, Resolution: {worst_combo['resolution']}, Mean Error: {worst_combo['mean_error']:.0f}m")
    
    # Sample distribution info
    print(f"\nSample Distribution:")
    print(f"  Total parameter combinations with data: {len(mean_flat)}")
    print(f"  FOV bins with data: {len(mean_pivot.dropna(how='all'))}")
    print(f"  Resolution values with data: {len(mean_pivot.dropna(how='all').columns)}")
    
    # Show sample counts for verification
    print(f"\nSample counts per resolution:")
    for res in sorted(df['resolution'].unique()):
        count = len(df[df['resolution'] == res])
        print(f"  {res}px: {count:,} samples")
    
    plt.show()
    
    return output_file

if __name__ == "__main__":
    # Path to the CSV file
    csv_file = "/home/josh/husky_satellite_lab/found/tools/simulator/data/default_full_20250805_004728/distance_analysis_results_clean.csv"
    
    if Path(csv_file).exists():
        create_error_heatmaps(csv_file)
    else:
        print(f"Error: CSV file not found at {csv_file}")
