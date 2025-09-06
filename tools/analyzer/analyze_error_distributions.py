#!/usr/bin/env python3
"""
Analyze the shape of error distributions showing how predicted positions 
fall around true positions. Uses signed error values to show bias patterns.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from scipy import stats

def analyze_error_distributions(csv_file_path):
    """
    Analyze error distribution shapes and bias patterns.
    
    Args:
        csv_file_path: Path to the CSV file with distance analysis results
    """
    print(f"Reading data from {csv_file_path}")
    df = pd.read_csv(csv_file_path)
    
    # Calculate altitude and signed error
    earth_radius = 6371000  # meters
    df['altitude'] = df['true_distance'] - earth_radius
    df['signed_error'] = df['measured_distance'] - df['true_distance']  # Positive = overestimate, Negative = underestimate
    df['signed_error_percentage'] = (df['signed_error'] / df['altitude']) * 100
    
    print(f"Total samples: {len(df)}")
    print(f"Error range: {df['signed_error'].min():.0f}m to {df['signed_error'].max():.0f}m")
    print(f"Mean error: {df['signed_error'].mean():.0f}m (positive = overestimate)")
    print(f"Standard deviation: {df['signed_error'].std():.0f}m")
    
    # Set up the figure with multiple subplots
    fig = plt.figure(figsize=(20, 16))
    
    # Create a 3x3 grid of subplots
    gs = fig.add_gridspec(3, 3, hspace=0.3, wspace=0.3)
    
    # 1. Overall error distribution histogram
    ax1 = fig.add_subplot(gs[0, 0])
    n, bins, patches = ax1.hist(df['signed_error']/1000, bins=50, alpha=0.7, color='skyblue', edgecolor='black')
    ax1.axvline(0, color='red', linestyle='--', linewidth=2, label='True Position')
    ax1.axvline(df['signed_error'].mean()/1000, color='orange', linestyle='-', linewidth=2, label=f'Mean Error: {df["signed_error"].mean()/1000:.1f}km')
    ax1.set_xlabel('Signed Error (km)')
    ax1.set_ylabel('Frequency')
    ax1.set_title('Overall Error Distribution\n(Positive = Overestimate, Negative = Underestimate)')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 2. Error distribution by FOV
    ax2 = fig.add_subplot(gs[0, 1])
    fov_values = sorted(df['fov_rounded'].unique())
    colors = plt.cm.viridis(np.linspace(0, 1, len(fov_values)))
    
    for i, fov in enumerate(fov_values):
        fov_data = df[df['fov_rounded'] == fov]['signed_error'] / 1000
        ax2.hist(fov_data, bins=30, alpha=0.6, color=colors[i], label=f'{fov}°', density=True)
    
    ax2.axvline(0, color='red', linestyle='--', linewidth=2)
    ax2.set_xlabel('Signed Error (km)')
    ax2.set_ylabel('Density')
    ax2.set_title('Error Distribution by FOV')
    ax2.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    ax2.grid(True, alpha=0.3)
    
    # 3. Error distribution by Resolution
    ax3 = fig.add_subplot(gs[0, 2])
    resolution_values = sorted(df['resolution'].unique())
    colors_res = plt.cm.plasma(np.linspace(0, 1, len(resolution_values)))
    
    for i, res in enumerate(resolution_values):
        res_data = df[df['resolution'] == res]['signed_error'] / 1000
        ax3.hist(res_data, bins=30, alpha=0.6, color=colors_res[i], label=f'{res}px', density=True)
    
    ax3.axvline(0, color='red', linestyle='--', linewidth=2)
    ax3.set_xlabel('Signed Error (km)')
    ax3.set_ylabel('Density')
    ax3.set_title('Error Distribution by Resolution')
    ax3.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    ax3.grid(True, alpha=0.3)
    
    # 4. Q-Q plot to test normality
    ax4 = fig.add_subplot(gs[1, 0])
    stats.probplot(df['signed_error'], dist="norm", plot=ax4)
    ax4.set_title('Q-Q Plot: Error vs Normal Distribution')
    ax4.grid(True, alpha=0.3)
    
    # 5. Box plots by FOV showing median, quartiles, and outliers
    ax5 = fig.add_subplot(gs[1, 1])
    fov_error_data = [df[df['fov_rounded'] == fov]['signed_error']/1000 for fov in fov_values]
    bp1 = ax5.boxplot(fov_error_data, labels=[f'{fov}°' for fov in fov_values], patch_artist=True)
    
    # Color the box plots
    for patch, color in zip(bp1['boxes'], colors):
        patch.set_facecolor(color)
        patch.set_alpha(0.7)
    
    ax5.axhline(0, color='red', linestyle='--', linewidth=2)
    ax5.set_xlabel('Field of View (degrees)')
    ax5.set_ylabel('Signed Error (km)')
    ax5.set_title('Error Distribution by FOV\n(Box plots show median, quartiles, outliers)')
    ax5.tick_params(axis='x', rotation=45)
    ax5.grid(True, alpha=0.3)
    
    # 6. Box plots by Resolution
    ax6 = fig.add_subplot(gs[1, 2])
    res_error_data = [df[df['resolution'] == res]['signed_error']/1000 for res in resolution_values]
    bp2 = ax6.boxplot(res_error_data, labels=[f'{res}px' for res in resolution_values], patch_artist=True)
    
    # Color the box plots
    for patch, color in zip(bp2['boxes'], colors_res):
        patch.set_facecolor(color)
        patch.set_alpha(0.7)
    
    ax6.axhline(0, color='red', linestyle='--', linewidth=2)
    ax6.set_xlabel('Resolution (pixels)')
    ax6.set_ylabel('Signed Error (km)')
    ax6.set_title('Error Distribution by Resolution\n(Box plots show median, quartiles, outliers)')
    ax6.tick_params(axis='x', rotation=45)
    ax6.grid(True, alpha=0.3)
    
    # 7. Violin plots showing full distribution shape by FOV
    ax7 = fig.add_subplot(gs[2, 0])
    fov_violin_data = []
    fov_violin_labels = []
    for fov in fov_values:
        fov_data = df[df['fov_rounded'] == fov]['signed_error'] / 1000
        if len(fov_data) > 10:  # Only include if enough data points
            fov_violin_data.append(fov_data)
            fov_violin_labels.append(f'{fov}°')
    
    vp1 = ax7.violinplot(fov_violin_data, showmeans=True, showmedians=True)
    ax7.axhline(0, color='red', linestyle='--', linewidth=2)
    ax7.set_xticks(range(1, len(fov_violin_labels) + 1))
    ax7.set_xticklabels(fov_violin_labels, rotation=45)
    ax7.set_xlabel('Field of View (degrees)')
    ax7.set_ylabel('Signed Error (km)')
    ax7.set_title('Error Distribution Shape by FOV\n(Violin plots show full probability density)')
    ax7.grid(True, alpha=0.3)
    
    # 8. Scatter plot: True vs Predicted distance
    ax8 = fig.add_subplot(gs[2, 1])
    # Sample data for readability (use every 10th point if too many)
    sample_idx = np.arange(0, len(df), max(1, len(df)//1000))
    sample_df = df.iloc[sample_idx]
    
    scatter = ax8.scatter(sample_df['true_distance']/1000, sample_df['measured_distance']/1000, 
                         c=sample_df['fov_rounded'], cmap='viridis', alpha=0.6, s=20)
    
    # Perfect prediction line (y=x)
    min_dist = min(df['true_distance'].min(), df['measured_distance'].min()) / 1000
    max_dist = max(df['true_distance'].max(), df['measured_distance'].max()) / 1000
    ax8.plot([min_dist, max_dist], [min_dist, max_dist], 'r--', linewidth=2, label='Perfect Prediction')
    
    ax8.set_xlabel('True Distance (km)')
    ax8.set_ylabel('Predicted Distance (km)')
    ax8.set_title('True vs Predicted Distance\n(Color = FOV)')
    plt.colorbar(scatter, ax=ax8, label='FOV (degrees)')
    ax8.legend()
    ax8.grid(True, alpha=0.3)
    
    # 9. Statistical summary table
    ax9 = fig.add_subplot(gs[2, 2])
    ax9.axis('off')
    
    # Calculate statistics by parameter
    stats_text = "Error Distribution Statistics\n" + "="*40 + "\n\n"
    
    # Overall statistics
    stats_text += f"Overall Statistics:\n"
    stats_text += f"  Mean Error: {df['signed_error'].mean():.0f}m\n"
    stats_text += f"  Median Error: {df['signed_error'].median():.0f}m\n"
    stats_text += f"  Std Deviation: {df['signed_error'].std():.0f}m\n"
    stats_text += f"  Skewness: {stats.skew(df['signed_error']):.3f}\n"
    stats_text += f"  Kurtosis: {stats.kurtosis(df['signed_error']):.3f}\n\n"
    
    # Test for normality
    shapiro_stat, shapiro_p = stats.shapiro(df['signed_error'].sample(min(5000, len(df))))
    stats_text += f"Normality Tests:\n"
    stats_text += f"  Shapiro-Wilk p-value: {shapiro_p:.2e}\n"
    stats_text += f"  {'Normal' if shapiro_p > 0.05 else 'Non-normal'} distribution\n\n"
    
    # Bias analysis
    overestimate_pct = (df['signed_error'] > 0).sum() / len(df) * 100
    underestimate_pct = (df['signed_error'] < 0).sum() / len(df) * 100
    stats_text += f"Bias Analysis:\n"
    stats_text += f"  Overestimates: {overestimate_pct:.1f}%\n"
    stats_text += f"  Underestimates: {underestimate_pct:.1f}%\n"
    stats_text += f"  Perfect predictions: {(df['signed_error'] == 0).sum()} samples\n\n"
    
    # Best/worst FOV
    fov_means = df.groupby('fov_rounded')['signed_error'].mean()
    best_fov = fov_means.abs().idxmin()
    worst_fov = fov_means.abs().idxmax()
    stats_text += f"FOV Performance:\n"
    stats_text += f"  Most accurate FOV: {best_fov}° ({fov_means[best_fov]:.0f}m bias)\n"
    stats_text += f"  Least accurate FOV: {worst_fov}° ({fov_means[worst_fov]:.0f}m bias)\n\n"
    
    # Best/worst resolution
    res_means = df.groupby('resolution')['signed_error'].mean()
    best_res = res_means.abs().idxmin()
    worst_res = res_means.abs().idxmax()
    stats_text += f"Resolution Performance:\n"
    stats_text += f"  Most accurate: {best_res}px ({res_means[best_res]:.0f}m bias)\n"
    stats_text += f"  Least accurate: {worst_res}px ({res_means[worst_res]:.0f}m bias)"
    
    ax9.text(0.05, 0.95, stats_text, transform=ax9.transAxes, fontsize=10, 
             verticalalignment='top', fontfamily='monospace',
             bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray", alpha=0.5))
    
    # Overall title
    fig.suptitle('Distance Error Distribution Analysis: How Predictions Fall Around True Positions\n' +
                f'Total Samples: {len(df):,} | Mean Bias: {df["signed_error"].mean():.0f}m | Std Dev: {df["signed_error"].std():.0f}m',
                fontsize=16, fontweight='bold', y=0.98)
    
    plt.tight_layout()
    
    # Save the plot
    output_dir = Path(csv_file_path).parent
    output_file = output_dir / 'error_distribution_analysis.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Error distribution analysis saved to: {output_file}")
    
    # Print detailed statistical analysis
    print("\n" + "="*80)
    print("DETAILED ERROR DISTRIBUTION ANALYSIS")
    print("="*80)
    
    print(f"\nDistribution Shape Analysis:")
    print(f"  Mean: {df['signed_error'].mean():.0f}m")
    print(f"  Median: {df['signed_error'].median():.0f}m")
    print(f"  Mode: {stats.mode(df['signed_error'], keepdims=True)[0][0]:.0f}m")
    print(f"  Skewness: {stats.skew(df['signed_error']):.3f} ({'Right-skewed' if stats.skew(df['signed_error']) > 0 else 'Left-skewed' if stats.skew(df['signed_error']) < 0 else 'Symmetric'})")
    print(f"  Kurtosis: {stats.kurtosis(df['signed_error']):.3f} ({'Heavy-tailed' if stats.kurtosis(df['signed_error']) > 0 else 'Light-tailed'})")
    
    print(f"\nBias Analysis:")
    print(f"  Systematic bias (mean): {df['signed_error'].mean():.0f}m")
    print(f"  Random error (std): {df['signed_error'].std():.0f}m")
    print(f"  Overestimates: {overestimate_pct:.1f}% of predictions")
    print(f"  Underestimates: {underestimate_pct:.1f}% of predictions")
    
    # Percentile analysis
    percentiles = [5, 25, 50, 75, 95]
    print(f"\nError Percentiles:")
    for p in percentiles:
        value = np.percentile(df['signed_error'], p)
        print(f"  {p:2d}th percentile: {value:8.0f}m")
    
    # Kurtosis analysis for high-resolution cameras (>1024px)
    high_res_df = df[df['resolution'] > 1024]
    print(f"\nHigh-Resolution Analysis (>1024px):")
    print(f"  Samples: {len(high_res_df)} ({len(high_res_df)/len(df)*100:.1f}% of total)")
    print(f"  Mean error: {high_res_df['signed_error'].mean():.0f}m")
    print(f"  Std deviation: {high_res_df['signed_error'].std():.0f}m")
    print(f"  Kurtosis: {stats.kurtosis(high_res_df['signed_error']):.3f} ({'Heavy-tailed' if stats.kurtosis(high_res_df['signed_error']) > 0 else 'Light-tailed'})")
    print(f"  Skewness: {stats.skew(high_res_df['signed_error']):.3f}")
    
    # Compare kurtosis by resolution category
    print(f"\nKurtosis by Resolution:")
    for res in sorted(df['resolution'].unique()):
        res_data = df[df['resolution'] == res]['signed_error']
        kurtosis_val = stats.kurtosis(res_data)
        print(f"  {res:4d}px: {kurtosis_val:7.3f} ({'Heavy' if kurtosis_val > 0 else 'Light'}-tailed)")
    
    print(f"\nKurtosis Comparison:")
    low_res_df = df[df['resolution'] <= 1024]
    print(f"  Low resolution (≤1024px):  {stats.kurtosis(low_res_df['signed_error']):.3f}")
    print(f"  High resolution (>1024px):  {stats.kurtosis(high_res_df['signed_error']):.3f}")
    print(f"  Difference: {stats.kurtosis(high_res_df['signed_error']) - stats.kurtosis(low_res_df['signed_error']):.3f}")
    
    plt.show()
    
    return output_file

if __name__ == "__main__":
    # Path to the CSV file
    csv_file = "/home/josh/husky_satellite_lab/found/tools/simulator/data/default_full_20250805_004728/distance_analysis_results_clean.csv"
    
    if Path(csv_file).exists():
        analyze_error_distributions(csv_file)
    else:
        print(f"Error: CSV file not found at {csv_file}")
