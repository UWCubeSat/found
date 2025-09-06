#!/usr/bin/env python3
"""
Create summary visualization charts for altitude error analysis.
Combines the statistics into easy-to-understand visual formats.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
import sys

def create_summary_charts(csv_path):
    """Create summary charts for the altitude error analysis."""
    # Load the data
    df = pd.read_csv(csv_path)
    
    # Calculate FOV and filter high-resolution data
    sensor_width = df['pixel_size'] * df['resolution']
    df['fov_degrees'] = np.degrees(2 * np.arctan(sensor_width / (2 * df['focal_length'])))
    df['fov_rounded'] = np.round(df['fov_degrees'])
    
    # Calculate altitude
    earth_radius = 6371000
    df['altitude'] = df['true_distance'] - earth_radius
    df['abs_error'] = np.abs(df['error'])
    
    # Filter high-resolution data (≥1024px)
    high_res_data = df[df['resolution'] >= 1024].copy()
    
    # Set up the plotting style
    plt.style.use('default')
    sns.set_palette("husl")
    
    # Create a comprehensive summary figure
    fig = plt.figure(figsize=(20, 16))
    
    # 1. Error vs Resolution (Top Left)
    ax1 = plt.subplot(3, 3, 1)
    resolution_stats = high_res_data.groupby('resolution')['abs_error'].agg(['mean', 'std', 'count'])
    plt.errorbar(resolution_stats.index, resolution_stats['mean'], 
                yerr=resolution_stats['std'], marker='o', capsize=5, linewidth=2)
    plt.xlabel('Resolution (pixels)')
    plt.ylabel('Mean Absolute Error (meters)')
    plt.title('Error vs Resolution (≥1024px)')
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    
    # Add sample count annotations
    for res, stats in resolution_stats.iterrows():
        plt.annotate(f'n={stats["count"]}', 
                    (res, stats['mean']), 
                    textcoords="offset points", 
                    xytext=(0,10), ha='center', fontsize=8)
    
    # 2. Error vs FOV (Top Center)
    ax2 = plt.subplot(3, 3, 2)
    fov_stats = high_res_data.groupby('fov_rounded')['abs_error'].agg(['mean', 'std', 'count'])
    plt.errorbar(fov_stats.index, fov_stats['mean'], 
                yerr=fov_stats['std'], marker='s', capsize=5, linewidth=2, color='orange')
    plt.xlabel('Field of View (degrees)')
    plt.ylabel('Mean Absolute Error (meters)')
    plt.title('Error vs FOV (≥1024px)')
    plt.grid(True, alpha=0.3)
    
    # 3. Best/Worst Combinations (Top Right)
    ax3 = plt.subplot(3, 3, 3)
    combo_stats = high_res_data.groupby(['resolution', 'fov_rounded'])['abs_error'].mean().reset_index()
    combo_stats = combo_stats.sort_values('abs_error')
    
    # Show top 5 best and worst
    best_5 = combo_stats.head(5)
    worst_5 = combo_stats.tail(5)
    
    y_pos = np.arange(10)
    combined = pd.concat([best_5, worst_5])
    combined['label'] = combined.apply(lambda x: f"{int(x['resolution'])}px, {x['fov_rounded']}°", axis=1)
    colors = ['green']*5 + ['red']*5
    
    plt.barh(y_pos, combined['abs_error'], color=colors, alpha=0.7)
    plt.yticks(y_pos, combined['label'])
    plt.xlabel('Mean Absolute Error (meters)')
    plt.title('Best/Worst Resolution-FOV Combinations')
    plt.grid(True, alpha=0.3, axis='x')
    
    # 4. Error as % of Altitude by Resolution (Middle Left)
    ax4 = plt.subplot(3, 3, 4)
    high_res_data['error_percent'] = (high_res_data['abs_error'] / high_res_data['altitude']) * 100
    res_percent_stats = high_res_data.groupby('resolution')['error_percent'].agg(['mean', 'std'])
    
    plt.errorbar(res_percent_stats.index, res_percent_stats['mean'], 
                yerr=res_percent_stats['std'], marker='o', capsize=5, linewidth=2, color='purple')
    plt.xlabel('Resolution (pixels)')
    plt.ylabel('Error as % of Altitude')
    plt.title('Error Percentage vs Resolution')
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    
    # 5. Error Distribution by Altitude Range (Middle Center)
    ax5 = plt.subplot(3, 3, 5)
    high_res_data['altitude_km'] = high_res_data['altitude'] / 1000
    altitude_bins = [400, 500, 600, 700, 800]
    high_res_data['altitude_bin'] = pd.cut(high_res_data['altitude_km'], bins=altitude_bins)
    
    alt_stats = high_res_data.groupby('altitude_bin')['abs_error'].agg(['mean', 'std'])
    alt_labels = ['400-500km', '500-600km', '600-700km', '700-800km']
    
    plt.errorbar(range(len(alt_labels)), alt_stats['mean'], 
                yerr=alt_stats['std'], marker='d', capsize=5, linewidth=2, color='brown')
    plt.xticks(range(len(alt_labels)), alt_labels, rotation=45)
    plt.xlabel('Altitude Range')
    plt.ylabel('Mean Absolute Error (meters)')
    plt.title('Error vs Altitude Range')
    plt.grid(True, alpha=0.3)
    
    # 6. FOV vs Error Heatmap (Middle Right)
    ax6 = plt.subplot(3, 3, 6)
    pivot_data = high_res_data.groupby(['resolution', 'fov_rounded'])['abs_error'].mean().unstack()
    sns.heatmap(pivot_data, annot=True, fmt='.0f', cmap='YlOrRd', 
                cbar_kws={'label': 'Mean Error (meters)'})
    plt.title('Error Heatmap: Resolution vs FOV')
    plt.xlabel('Field of View (degrees)')
    plt.ylabel('Resolution (pixels)')
    
    # 7. Error Variability (Bottom Left)
    ax7 = plt.subplot(3, 3, 7)
    variability_stats = high_res_data.groupby('resolution')['abs_error'].std()
    plt.bar(range(len(variability_stats)), variability_stats.values, 
           color='lightcoral', alpha=0.7)
    plt.xticks(range(len(variability_stats)), 
              [f"{int(res)}px" for res in variability_stats.index], rotation=45)
    plt.xlabel('Resolution')
    plt.ylabel('Error Standard Deviation (meters)')
    plt.title('Error Variability by Resolution')
    plt.grid(True, alpha=0.3, axis='y')
    
    # 8. Sample Distribution (Bottom Center)
    ax8 = plt.subplot(3, 3, 8)
    sample_counts = high_res_data.groupby(['resolution', 'fov_rounded']).size().unstack()
    sns.heatmap(sample_counts, annot=True, fmt='d', cmap='Blues', 
                cbar_kws={'label': 'Sample Count'})
    plt.title('Sample Distribution')
    plt.xlabel('Field of View (degrees)')
    plt.ylabel('Resolution (pixels)')
    
    # 9. Summary Statistics Table (Bottom Right)
    ax9 = plt.subplot(3, 3, 9)
    ax9.axis('off')
    
    # Create summary text
    total_samples = len(high_res_data)
    mean_error = high_res_data['abs_error'].mean()
    std_error = high_res_data['abs_error'].std()
    mean_percent = high_res_data['error_percent'].mean()
    
    best_combo = combo_stats.iloc[0]
    worst_combo = combo_stats.iloc[-1]
    
    summary_text = f"""
HIGH-RESOLUTION SUMMARY (≥1024px)
{'='*35}

Total Samples: {total_samples:,}
Mean Error: {mean_error:.1f} ± {std_error:.1f} m
Mean Error %: {mean_percent:.2f}% of altitude

BEST COMBINATION:
{int(best_combo['resolution'])}px, {best_combo['fov_rounded']}°
Error: {best_combo['abs_error']:.1f} m

WORST COMBINATION:
{int(worst_combo['resolution'])}px, {worst_combo['fov_rounded']}°
Error: {worst_combo['abs_error']:.1f} m

RESOLUTION RANGE:
{high_res_data['resolution'].min()} - {high_res_data['resolution'].max()} pixels

FOV RANGE:
{high_res_data['fov_rounded'].min():.0f} - {high_res_data['fov_rounded'].max():.0f} degrees

ALTITUDE RANGE:
{high_res_data['altitude'].min()/1000:.0f} - {high_res_data['altitude'].max()/1000:.0f} km
    """
    
    plt.text(0.05, 0.95, summary_text, transform=ax9.transAxes, 
             fontsize=10, verticalalignment='top', fontfamily='monospace',
             bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray", alpha=0.8))
    
    plt.tight_layout()
    
    # Save the comprehensive summary
    output_path = Path(csv_path).parent / 'comprehensive_error_analysis_summary.png'
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Comprehensive summary chart saved to {output_path}")
    plt.show()
    
    # Create a second figure focusing on altitude analysis
    fig2, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    
    # Chart 1: Error vs Altitude by Resolution
    for resolution in sorted(high_res_data['resolution'].unique()):
        res_data = high_res_data[high_res_data['resolution'] == resolution]
        ax1.scatter(res_data['altitude']/1000, res_data['abs_error'], 
                   alpha=0.6, label=f'{resolution}px', s=20)
    
    ax1.set_xlabel('Altitude (km)')
    ax1.set_ylabel('Absolute Error (meters)')
    ax1.set_title('Error vs Altitude by Resolution')
    ax1.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    ax1.grid(True, alpha=0.3)
    
    # Chart 2: Error Percentage by FOV and Resolution
    for resolution in sorted(high_res_data['resolution'].unique())[:3]:  # Show top 3 resolutions
        res_data = high_res_data[high_res_data['resolution'] == resolution]
        fov_means = res_data.groupby('fov_rounded')['error_percent'].mean()
        ax2.plot(fov_means.index, fov_means.values, 
                marker='o', linewidth=2, label=f'{resolution}px')
    
    ax2.set_xlabel('Field of View (degrees)')
    ax2.set_ylabel('Error as % of Altitude')
    ax2.set_title('Error Percentage by FOV (Top 3 Resolutions)')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # Chart 3: Altitude Range Performance
    altitude_bins = [400, 500, 600, 700, 800]
    high_res_data['altitude_bin'] = pd.cut(high_res_data['altitude_km'], bins=altitude_bins)
    alt_performance = high_res_data.groupby(['altitude_bin', 'resolution'])['abs_error'].mean().unstack()
    
    alt_performance.plot(kind='bar', ax=ax3, width=0.8)
    ax3.set_xlabel('Altitude Range (km)')
    ax3.set_ylabel('Mean Absolute Error (meters)')
    ax3.set_title('Performance by Altitude Range and Resolution')
    ax3.legend(title='Resolution', bbox_to_anchor=(1.05, 1), loc='upper left')
    ax3.grid(True, alpha=0.3, axis='y')
    ax3.tick_params(axis='x', rotation=45)
    
    # Chart 4: Error Correlation with Altitude
    correlation_data = []
    for res in sorted(high_res_data['resolution'].unique()):
        for fov in sorted(high_res_data['fov_rounded'].unique()):
            subset = high_res_data[(high_res_data['resolution'] == res) & 
                                  (high_res_data['fov_rounded'] == fov)]
            if len(subset) > 10:  # Only include combinations with enough samples
                corr = subset['altitude'].corr(subset['abs_error'])
                correlation_data.append({
                    'resolution': res,
                    'fov': fov,
                    'correlation': corr,
                    'sample_count': len(subset)
                })
    
    corr_df = pd.DataFrame(correlation_data)
    corr_pivot = corr_df.pivot(index='resolution', columns='fov', values='correlation')
    
    sns.heatmap(corr_pivot, annot=True, fmt='.3f', cmap='RdBu_r', center=0,
                ax=ax4, cbar_kws={'label': 'Correlation with Altitude'})
    ax4.set_title('Error-Altitude Correlation by Resolution-FOV')
    ax4.set_xlabel('Field of View (degrees)')
    ax4.set_ylabel('Resolution (pixels)')
    
    plt.tight_layout()
    
    # Save the altitude-focused analysis
    output_path2 = Path(csv_path).parent / 'altitude_focused_analysis.png'
    plt.savefig(output_path2, dpi=300, bbox_inches='tight')
    print(f"Altitude-focused analysis chart saved to {output_path2}")
    plt.show()

if __name__ == "__main__":
    csv_path = "/home/josh/husky_satellite_lab/found/tools/simulator/data/default_full_20250805_004728/distance_analysis_results_clean.csv"
    create_summary_charts(csv_path)
