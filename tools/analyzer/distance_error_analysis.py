#!/usr/bin/env python3
"""
Distance Error Analysis Tool

This script analyzes simulation results to understand how measurement error
is affected by various parameters including:
- Distance (true distance)
- Field of View (derived from focal length and sensor size)
- Sensor size (pixel size and resolution)
- Viewing angle (orientation)

Author: Generated for distance measurement analysis
Date: 2025-08-05
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
import argparse
from typing import Tuple, Dict, List
import warnings
warnings.filterwarnings('ignore')

class DistanceErrorAnalyzer:
    """Analyzes distance measurement errors from simulation results."""
    
    def __init__(self, csv_path: str):
        """Initialize the analyzer with CSV data."""
        self.csv_path = Path(csv_path)
        self.df = None
        self.load_data()
        self.preprocess_data()
    
    def load_data(self):
        """Load CSV data into a pandas DataFrame."""
        try:
            self.df = pd.read_csv(self.csv_path)
            print(f"Loaded {len(self.df)} records from {self.csv_path}")
        except Exception as e:
            raise ValueError(f"Error loading CSV file: {e}")
    
    def preprocess_data(self):
        """Preprocess data to extract useful features."""
        # Calculate field of view (FOV) in degrees
        # FOV = 2 * arctan(sensor_width / (2 * focal_length))
        # Assuming square sensor, sensor_width = pixel_size * resolution
        sensor_width = self.df['pixel_size'] * self.df['resolution']
        self.df['fov_radians'] = 2 * np.arctan(sensor_width / (2 * self.df['focal_length']))
        self.df['fov_degrees'] = np.degrees(self.df['fov_radians'])
        
        # Round FOV to nearest degree for grouping
        self.df['fov_rounded'] = np.round(self.df['fov_degrees'])
        
        # Calculate altitude from true_distance (assuming Earth radius = 6371000 m)
        earth_radius = 6371000  # meters
        self.df['altitude'] = self.df['true_distance'] - earth_radius
        
        # Extract orientation components
        orientation_parts = self.df['orientation'].str.split(expand=True).astype(float)
        self.df['orientation_x'] = orientation_parts[0]
        self.df['orientation_y'] = orientation_parts[1] 
        self.df['orientation_z'] = orientation_parts[2]
        
        # Calculate total orientation angle magnitude
        self.df['orientation_magnitude'] = np.sqrt(
            self.df['orientation_x']**2 + 
            self.df['orientation_y']**2 + 
            self.df['orientation_z']**2
        )
        
        # Calculate absolute error for analysis
        self.df['abs_error'] = np.abs(self.df['error'])
        self.df['abs_relative_error'] = np.abs(self.df['relative_error'])
        
        # Identify and separate extreme outliers (>3 std deviations)
        self.identify_and_separate_outliers()
        
        # Create categorical variables for analysis
        self.df_clean['resolution_category'] = pd.Categorical(self.df_clean['resolution'])
        self.df_clean['focal_length_category'] = pd.Categorical(self.df_clean['focal_length'])
        
        print("Data preprocessing completed")
        print(f"Original samples: {len(self.df)}")
        print(f"Clean samples (after removing 3σ outliers): {len(self.df_clean)}")
        print(f"Extreme outliers removed: {len(self.extreme_outliers)}")
        print(f"FOV range: {self.df_clean['fov_degrees'].min():.2f}° to {self.df_clean['fov_degrees'].max():.2f}°")
        print(f"FOV rounded values: {sorted(self.df_clean['fov_rounded'].unique())}")
        print(f"Resolution range: {self.df_clean['resolution'].min()} to {self.df_clean['resolution'].max()}")
        print(f"Focal length range: {self.df_clean['focal_length'].min():.6f} to {self.df_clean['focal_length'].max():.6f}")
        print(f"Altitude range: {self.df_clean['altitude'].min():.0f} to {self.df_clean['altitude'].max():.0f} meters")
    
    def identify_and_separate_outliers(self):
        """Identify and separate extreme outliers (>3 standard deviations) from the main dataset."""
        # Calculate mean and standard deviation of absolute relative error
        mean_error = self.df['abs_relative_error'].mean()
        std_error = self.df['abs_relative_error'].std()
        
        # Define threshold for extreme outliers (3 standard deviations)
        threshold = mean_error + 3 * std_error
        
        # Separate extreme outliers from clean data
        extreme_outlier_mask = self.df['abs_relative_error'] > threshold
        self.extreme_outliers = self.df[extreme_outlier_mask].copy()
        self.df_clean = self.df[~extreme_outlier_mask].copy()
        
        # Add generation commands to extreme outliers
        if len(self.extreme_outliers) > 0:
            self.extreme_outliers['combo'] = self.extreme_outliers['image_file'].str.extract(r'combo(\d+)')
            self.extreme_outliers['angle'] = self.extreme_outliers['image_file'].str.extract(r'angle(\d+)')
            self.extreme_outliers['generation_command'] = self.extreme_outliers.apply(self._reconstruct_command, axis=1)
            self.extreme_outliers['outlier_type'] = 'extreme_3sigma'
            self.extreme_outliers['threshold_used'] = threshold
        
        print(f"Extreme outlier threshold (mean + 3σ): {threshold:.4f}")
        print(f"Mean error: {mean_error:.4f}, Std error: {std_error:.4f}")
    
    def analyze_error_by_parameter(self, parameter: str, error_metric: str = 'abs_relative_error') -> Dict:
        """Analyze error statistics by a given parameter using clean data."""
        stats = self.df_clean.groupby(parameter)[error_metric].agg([
            'mean', 'std', 'min', 'max', 'median'
        ]).round(6)
        
        # Handle correlation calculation with variance check
        if self.df_clean[parameter].nunique() <= 1:
            correlation = None  # No correlation possible with no variance
        else:
            correlation = self.df_clean[parameter].corr(self.df_clean[error_metric])
        
        return {
            'parameter': parameter,
            'stats': stats,
            'correlation': correlation
        }
    
    def plot_error_vs_parameters(self, save_plots: bool = True):
        """Create five separate charts showing error relationships using clean data."""
        # Set up the plotting style
        plt.style.use('default')
        sns.set_palette("husl")
        
        # Chart 1: Error vs Field of View
        plt.figure(figsize=(10, 6))
        if self.df_clean['fov_rounded'].nunique() > 1:
            fov_groups = self.df_clean.groupby('fov_rounded')['abs_relative_error']
            fov_means = fov_groups.mean()
            fov_stds = fov_groups.std()
            
            plt.errorbar(fov_means.index, fov_means.values, 
                        yerr=fov_stds.values, marker='s', capsize=5, color='orange')
        else:
            plt.scatter(self.df_clean['fov_rounded'], self.df_clean['abs_relative_error'], 
                       alpha=0.6, color='orange')
        
        plt.xlabel('Field of View (degrees)')
        plt.ylabel('Absolute Relative Error')
        plt.title('Error vs Field of View')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'error_vs_fov.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"FOV plot saved to {output_path}")
        plt.show()
        
        # Chart 2: Error vs Resolution
        plt.figure(figsize=(10, 6))
        resolution_groups = self.df_clean.groupby('resolution')['abs_relative_error']
        resolution_means = resolution_groups.mean()
        resolution_stds = resolution_groups.std()
        
        plt.errorbar(resolution_means.index, resolution_means.values, 
                    yerr=resolution_stds.values, marker='o', capsize=5, color='blue')
        plt.xlabel('Resolution (pixels)')
        plt.ylabel('Absolute Relative Error')
        plt.title('Error vs Resolution')
        plt.xscale('log')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'error_vs_resolution.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"Resolution plot saved to {output_path}")
        plt.show()
        
        # Chart 3: Error in meters vs Altitude
        plt.figure(figsize=(10, 6))
        plt.scatter(self.df_clean['altitude'], self.df_clean['abs_error'], 
                   alpha=0.6, c=self.df_clean['fov_rounded'], cmap='plasma')
        plt.xlabel('Altitude (meters)')
        plt.ylabel('Absolute Error (meters)')
        plt.title('Error in Meters vs Altitude')
        plt.grid(True, alpha=0.3)
        
        # Add colorbar to show FOV values
        cbar = plt.colorbar()
        cbar.set_label('Field of View (degrees)')
        
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'error_vs_altitude.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"Altitude plot saved to {output_path}")
        plt.show()
        
        # Chart 4: FOV Error Scaled by Altitude (separate line for each resolution)
        plt.figure(figsize=(12, 8))
        # Calculate error as percentage of altitude
        self.df_clean['error_percent_altitude'] = (self.df_clean['abs_error'] / self.df_clean['altitude']) * 100
        
        if self.df_clean['fov_rounded'].nunique() > 1:
            # Get unique resolutions and create a color map
            resolutions = sorted(self.df_clean['resolution'].unique())
            colors = plt.cm.tab10(np.linspace(0, 1, len(resolutions)))
            
            for i, resolution in enumerate(resolutions):
                # Filter data for this resolution
                res_data = self.df_clean[self.df_clean['resolution'] == resolution]
                
                if len(res_data) > 0:
                    # Group by FOV for this resolution
                    fov_groups = res_data.groupby('fov_rounded')['error_percent_altitude']
                    fov_means = fov_groups.mean()
                    fov_stds = fov_groups.std()
                    
                    # Plot line with error bars for this resolution
                    plt.errorbar(fov_means.index, fov_means.values, 
                                yerr=fov_stds.values, marker='o', capsize=3, 
                                color=colors[i], label=f'{resolution}px', linewidth=2)
            
            plt.legend(title='Resolution', bbox_to_anchor=(1.05, 1), loc='upper left')
        else:
            # Fallback if only one FOV value
            for i, resolution in enumerate(resolutions):
                res_data = self.df_clean[self.df_clean['resolution'] == resolution]
                if len(res_data) > 0:
                    plt.scatter(res_data['fov_rounded'], res_data['error_percent_altitude'], 
                               alpha=0.6, color=colors[i], label=f'{resolution}px')
            plt.legend(title='Resolution')
        
        plt.xlabel('Field of View (degrees)')
        plt.ylabel('Error as % of Altitude')
        plt.title('FOV Error Scaled by Altitude (by Resolution)')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'fov_error_scaled_by_altitude.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"FOV error scaled by altitude plot saved to {output_path}")
        plt.show()
        
        # Chart 5: Resolution Error Scaled by Altitude
        plt.figure(figsize=(10, 6))
        resolution_groups_scaled = self.df_clean.groupby('resolution')['error_percent_altitude']
        resolution_means_scaled = resolution_groups_scaled.mean()
        resolution_stds_scaled = resolution_groups_scaled.std()
        
        plt.errorbar(resolution_means_scaled.index, resolution_means_scaled.values, 
                    yerr=resolution_stds_scaled.values, marker='o', capsize=5, color='red')
        plt.xlabel('Resolution (pixels)')
        plt.ylabel('Error as % of Altitude')
        plt.title('Resolution Error Scaled by Altitude')
        plt.xscale('log')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'resolution_error_scaled_by_altitude.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"Resolution error scaled by altitude plot saved to {output_path}")
        plt.show()
        
        # Chart 6: FOV Error for High Resolution Only (1024px and above)
        plt.figure(figsize=(12, 8))
        # Filter data to only include resolutions >= 1024
        high_res_data = self.df_clean[self.df_clean['resolution'] >= 1024]
        
        if len(high_res_data) > 0 and high_res_data['fov_rounded'].nunique() > 1:
            # Get unique resolutions >= 1024 and create a color map
            resolutions = sorted(high_res_data['resolution'].unique())
            colors = plt.cm.tab10(np.linspace(0, 1, len(resolutions)))
            
            for i, resolution in enumerate(resolutions):
                # Filter data for this resolution
                res_data = high_res_data[high_res_data['resolution'] == resolution]
                
                if len(res_data) > 0:
                    # Group by FOV for this resolution
                    fov_groups = res_data.groupby('fov_rounded')['error_percent_altitude']
                    fov_means = fov_groups.mean()
                    fov_stds = fov_groups.std()
                    
                    # Plot line with error bars for this resolution
                    plt.errorbar(fov_means.index, fov_means.values, 
                                yerr=fov_stds.values, marker='o', capsize=3, 
                                color=colors[i], label=f'{resolution}px', linewidth=2)
            
            plt.legend(title='Resolution (≥1024px)', bbox_to_anchor=(1.05, 1), loc='upper left')
        else:
            # Fallback if no high resolution data or only one FOV value
            if len(high_res_data) > 0:
                resolutions = sorted(high_res_data['resolution'].unique())
                colors = plt.cm.tab10(np.linspace(0, 1, len(resolutions)))
                for i, resolution in enumerate(resolutions):
                    res_data = high_res_data[high_res_data['resolution'] == resolution]
                    if len(res_data) > 0:
                        plt.scatter(res_data['fov_rounded'], res_data['error_percent_altitude'], 
                                   alpha=0.6, color=colors[i], label=f'{resolution}px')
                plt.legend(title='Resolution (≥1024px)')
            else:
                plt.text(0.5, 0.5, 'No data with resolution ≥ 1024px', 
                        transform=plt.gca().transAxes, ha='center', va='center')
        
        plt.xlabel('Field of View (degrees)')
        plt.ylabel('Error as % of Altitude')
        plt.title('FOV Error for High Resolution (≥1024px)')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_plots:
            output_path = self.csv_path.parent / 'fov_error_high_resolution.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"FOV error for high resolution plot saved to {output_path}")
        plt.show()
    
    def create_correlation_matrix(self, save_plot: bool = True):
        """Create correlation matrix heatmap using clean data."""
        # Select numerical columns for correlation analysis
        numerical_cols = [
            'true_distance', 'measured_distance', 'error', 'relative_error',
            'abs_error', 'abs_relative_error', 'nadir_deviation', 
            'focal_length', 'pixel_size', 'resolution', 'num_pixels',
            'orientation_magnitude'
        ]
        
        # Only include FOV if it has variance
        if self.df_clean['fov_rounded'].nunique() > 1:
            numerical_cols.append('fov_rounded')
        
        correlation_matrix = self.df_clean[numerical_cols].corr()
        
        plt.figure(figsize=(12, 10))
        sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                   square=True, fmt='.3f', cbar_kws={'label': 'Correlation Coefficient'})
        
        title = 'Parameter Correlation Matrix (Excluding 3σ Outliers)'
        if self.df_clean['fov_rounded'].nunique() <= 1:
            title += '\n(FOV excluded - no variance)'
        plt.title(title, fontsize=14, fontweight='bold')
        plt.tight_layout()
        
        if save_plot:
            output_path = self.csv_path.parent / 'correlation_matrix_clean.png'
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
            print(f"Clean correlation matrix saved to {output_path}")
        
        plt.show()
        
        return correlation_matrix
    
    def identify_outliers(self, threshold_percentile: float = 95) -> pd.DataFrame:
        """Identify outliers based on relative error from clean data and extract generation parameters."""
        # Define outliers as samples above the specified percentile from clean data
        error_threshold = np.percentile(self.df_clean['abs_relative_error'], threshold_percentile)
        outliers = self.df_clean[self.df_clean['abs_relative_error'] >= error_threshold].copy()
        
        # Sort by error (worst first)
        outliers = outliers.sort_values('abs_relative_error', ascending=False)
        
        # Extract generation parameters from image filename and data
        outliers['combo'] = outliers['image_file'].str.extract(r'combo(\d+)')
        outliers['angle'] = outliers['image_file'].str.extract(r'angle(\d+)')
        
        # Create command reconstruction
        outliers['generation_command'] = outliers.apply(self._reconstruct_command, axis=1)
        outliers['outlier_type'] = 'percentile_based'
        outliers['threshold_used'] = error_threshold
        
        return outliers[['image_file', 'abs_relative_error', 'resolution', 'fov_rounded', 
                        'nadir_deviation', 'orientation_magnitude', 'combo', 'angle', 
                        'outlier_type', 'threshold_used', 'generation_command']]
    
    def _reconstruct_command(self, row) -> str:
        """Reconstruct the likely command used to generate this image."""
        # This is based on typical simulation command patterns
        # You may need to adjust this based on your actual simulation setup
        return (f"simulate --resolution {int(row['resolution'])} "
                f"--focal-length {row['focal_length']:.6f} "
                f"--pixel-size {row['pixel_size']:.0e} "
                f"--orientation \"{row['orientation_x']:.6f} {row['orientation_y']:.6f} {row['orientation_z']:.6f}\" "
                f"--combo {row['combo']} --angle {row['angle']}")
    
    def analyze_outliers(self, threshold_percentile: float = 95) -> Dict:
        """Comprehensive outlier analysis using clean data and including extreme outliers."""
        # Get outliers from clean data
        clean_outliers = self.identify_outliers(threshold_percentile)
        
        # Combine with extreme outliers for complete picture
        all_outliers = []
        
        if len(clean_outliers) > 0:
            all_outliers.append(clean_outliers)
        
        if len(self.extreme_outliers) > 0:
            extreme_for_analysis = self.extreme_outliers[['image_file', 'abs_relative_error', 'resolution', 
                                                         'fov_rounded', 'nadir_deviation', 'orientation_magnitude', 
                                                         'combo', 'angle', 'outlier_type', 'threshold_used', 
                                                         'generation_command']].copy()
            all_outliers.append(extreme_for_analysis)
        
        combined_outliers = pd.concat(all_outliers, ignore_index=True) if all_outliers else pd.DataFrame()
        
        if len(combined_outliers) > 0:
            combined_outliers = combined_outliers.sort_values('abs_relative_error', ascending=False)
        
        analysis = {
            'clean_data_threshold': np.percentile(self.df_clean['abs_relative_error'], threshold_percentile) if len(self.df_clean) > 0 else 0,
            'clean_outliers': len(clean_outliers),
            'extreme_outliers': len(self.extreme_outliers),
            'total_outliers': len(combined_outliers),
            'worst_error': combined_outliers['abs_relative_error'].max() if len(combined_outliers) > 0 else 0,
            'outliers_by_resolution': combined_outliers.groupby('resolution').size().to_dict() if len(combined_outliers) > 0 else {},
            'outliers_by_fov': combined_outliers.groupby('fov_rounded').size().to_dict() if len(combined_outliers) > 0 else {},
            'outlier_details': combined_outliers
        }
        
        return analysis
    
    def generate_summary_report(self) -> str:
        """Generate a comprehensive summary report using clean data."""
        report = []
        report.append("=" * 80)
        report.append("DISTANCE MEASUREMENT ERROR ANALYSIS REPORT")
        report.append("=" * 80)
        report.append(f"Dataset: {self.csv_path.name}")
        report.append(f"Total original samples: {len(self.df)}")
        report.append(f"Clean samples (excluding 3σ outliers): {len(self.df_clean)}")
        report.append(f"Extreme outliers removed: {len(self.extreme_outliers)}")
        report.append("")
        
        # Basic statistics from clean data
        report.append("BASIC ERROR STATISTICS (CLEAN DATA):")
        report.append("-" * 40)
        error_stats = self.df_clean['abs_relative_error'].describe()
        for stat, value in error_stats.items():
            report.append(f"{stat:>15}: {value:.6f}")
        report.append("")
        
        # Parameter ranges from clean data
        report.append("PARAMETER RANGES (CLEAN DATA):")
        report.append("-" * 40)
        parameters = {
            'FOV (degrees)': 'fov_rounded',
            'Resolution (px)': 'resolution', 
            'Altitude (m)': 'altitude',
            'Focal Length (m)': 'focal_length',
            'Nadir Deviation': 'nadir_deviation',
            'Orientation Mag': 'orientation_magnitude'
        }
        
        for param_name, param_col in parameters.items():
            if param_name == 'FOV (degrees)':
                unique_vals = sorted(self.df_clean[param_col].unique())
                report.append(f"{param_name:>15}: {unique_vals}")
            else:
                min_val = self.df_clean[param_col].min()
                max_val = self.df_clean[param_col].max()
                report.append(f"{param_name:>15}: {min_val:.6f} to {max_val:.6f}")
        report.append("")
        
        # Correlation analysis (use rounded FOV)
        correlation_params = {
            'FOV (degrees)': 'fov_rounded',
            'Resolution (px)': 'resolution', 
            'Altitude (m)': 'altitude',
            'Focal Length (m)': 'focal_length',
            'Nadir Deviation': 'nadir_deviation',
            'Orientation Mag': 'orientation_magnitude'
        }
        
        report.append("CORRELATION WITH ABSOLUTE RELATIVE ERROR (CLEAN DATA):")
        report.append("-" * 40)
        correlations = {}
        for param_name, param_col in correlation_params.items():
            # Check if parameter has variance (avoid NaN correlations)
            if self.df_clean[param_col].nunique() <= 1:
                corr = 0.0  # No correlation if no variance
                report.append(f"{param_name:>15}: {'N/A (no variance)':>20}")
            else:
                corr = self.df_clean[param_col].corr(self.df_clean['abs_relative_error'])
                correlations[param_name] = corr
                report.append(f"{param_name:>15}: {corr:>8.4f}")
        report.append("")
        
        # Error by resolution analysis using clean data
        report.append("ERROR BY RESOLUTION (CLEAN DATA):")
        report.append("-" * 40)
        resolution_analysis = self.analyze_error_by_parameter('resolution')
        for res, stats in resolution_analysis['stats'].iterrows():
            report.append(f"Resolution {res:>4}px: Mean={stats['mean']:.4f}, Std={stats['std']:.4f}")
        report.append("")
        
        # Error by FOV analysis using clean data
        if self.df_clean['fov_rounded'].nunique() > 1:
            report.append("ERROR BY FIELD OF VIEW (CLEAN DATA):")
            report.append("-" * 40)
            fov_analysis = self.analyze_error_by_parameter('fov_rounded')
            for fov, stats in fov_analysis['stats'].iterrows():
                report.append(f"FOV {fov:>4}°: Mean={stats['mean']:.4f}, Std={stats['std']:.4f}")
            report.append("")
        
        # Key findings using clean data
        report.append("KEY FINDINGS (CLEAN DATA):")
        report.append("-" * 40)
        
        # Find parameter with strongest correlation (excluding those with no variance)
        if correlations:
            strongest_corr = max(correlations.items(), key=lambda x: abs(x[1]))
            report.append(f"• Strongest correlation with error: {strongest_corr[0]} ({strongest_corr[1]:.4f})")
        else:
            report.append("• No parameters show meaningful correlation (insufficient variance)")
        
        # Best and worst resolution performance
        res_means = self.df_clean.groupby('resolution')['abs_relative_error'].mean()
        best_res = res_means.idxmin()
        worst_res = res_means.idxmax()
        report.append(f"• Best performing resolution: {best_res}px (error: {res_means[best_res]:.4f})")
        report.append(f"• Worst performing resolution: {worst_res}px (error: {res_means[worst_res]:.4f})")
        
        # Best and worst FOV performance (if applicable)
        if self.df_clean['fov_rounded'].nunique() > 1:
            fov_means = self.df_clean.groupby('fov_rounded')['abs_relative_error'].mean()
            best_fov = fov_means.idxmin()
            worst_fov = fov_means.idxmax()
            report.append(f"• Best performing FOV: {best_fov}° (error: {fov_means[best_fov]:.4f})")
            report.append(f"• Worst performing FOV: {worst_fov}° (error: {fov_means[worst_fov]:.4f})")
        
        # FOV impact
        fov_corr = correlations.get('FOV (degrees)', 0)
        if self.df_clean['fov_rounded'].nunique() <= 1:
            report.append(f"• Field of view: All samples have same FOV ({self.df_clean['fov_rounded'].iloc[0]:.0f}°) - no variance to analyze")
        elif abs(fov_corr) > 0.3:
            direction = "increases" if fov_corr > 0 else "decreases"
            report.append(f"• Field of view significantly affects error (correlation: {fov_corr:.4f})")
            report.append(f"  Error {direction} with larger FOV")
        
        # Outlier analysis
        outlier_analysis = self.analyze_outliers(95)
        report.append("")
        report.append("OUTLIER ANALYSIS:")
        report.append("-" * 40)
        report.append(f"• Extreme outliers (>3σ): {outlier_analysis['extreme_outliers']}")
        report.append(f"• Clean data outliers (top 5%): {outlier_analysis['clean_outliers']}")
        report.append(f"• Total outliers: {outlier_analysis['total_outliers']}")
        if outlier_analysis['total_outliers'] > 0:
            report.append(f"• Worst error: {outlier_analysis['worst_error']:.4f}")
        
        if outlier_analysis['outliers_by_resolution']:
            report.append("• All outliers by resolution:")
            for res, count in outlier_analysis['outliers_by_resolution'].items():
                report.append(f"  {res}px: {count} samples")
        
        if outlier_analysis['outliers_by_fov']:
            report.append("• All outliers by FOV:")
            for fov, count in outlier_analysis['outliers_by_fov'].items():
                report.append(f"  {fov}°: {count} samples")
        
        # Show extreme outliers separately
        if len(self.extreme_outliers) > 0:
            report.append("")
            report.append("EXTREME OUTLIERS (>3σ) WITH GENERATION COMMANDS:")
            report.append("-" * 40)
            for idx, row in self.extreme_outliers.head(5).iterrows():
                report.append(f"• {row['image_file']} (Error: {row['abs_relative_error']:.4f})")
                report.append(f"  Resolution: {row['resolution']}px, FOV: {row['fov_rounded']}°")
                report.append(f"  Command: {row['generation_command']}")
                report.append("")
        
        report.append("")
        report.append("=" * 80)
        
        return "\n".join(report)
    
    def print_altitude_error_statistics(self):
        """Print detailed statistics of absolute error vs altitude, broken down by FOV and resolution (≥1024px)."""
        print("\n" + "="*80)
        print("ABSOLUTE ERROR vs ALTITUDE STATISTICS (High Resolution ≥1024px)")
        print("="*80)
        
        # Filter data to only include resolutions >= 1024
        high_res_data = self.df_clean[self.df_clean['resolution'] >= 1024].copy()
        
        if len(high_res_data) == 0:
            print("No data found with resolution ≥ 1024px")
            return
        
        print(f"Total high-resolution samples: {len(high_res_data)}")
        print(f"Resolutions included: {sorted(high_res_data['resolution'].unique())}")
        print(f"FOV values: {sorted(high_res_data['fov_rounded'].unique())}")
        print()
        
        # Create altitude bins for better analysis
        high_res_data['altitude_km'] = high_res_data['altitude'] / 1000
        altitude_bins = [0, 200, 300, 400, 500, 600, 700, 800, 900, 1000, float('inf')]
        altitude_labels = ['0-200km', '200-300km', '300-400km', '400-500km', '500-600km', 
                          '600-700km', '700-800km', '800-900km', '900-1000km', '1000+km']
        high_res_data['altitude_bin'] = pd.cut(high_res_data['altitude_km'], 
                                              bins=altitude_bins, labels=altitude_labels, right=False)
        
        # Group by resolution, FOV, and altitude bin
        for resolution in sorted(high_res_data['resolution'].unique()):
            res_data = high_res_data[high_res_data['resolution'] == resolution]
            print(f"\nRESOLUTION: {resolution}px")
            print("-" * 60)
            
            for fov in sorted(res_data['fov_rounded'].unique()):
                fov_data = res_data[res_data['fov_rounded'] == fov]
                if len(fov_data) == 0:
                    continue
                    
                print(f"\n  Field of View: {fov}° ({len(fov_data)} samples)")
                print("  " + "-" * 50)
                
                # Overall statistics for this FOV/resolution combination
                overall_stats = {
                    'mean_error': fov_data['abs_error'].mean(),
                    'std_error': fov_data['abs_error'].std(),
                    'median_error': fov_data['abs_error'].median(),
                    'min_error': fov_data['abs_error'].min(),
                    'max_error': fov_data['abs_error'].max(),
                    'mean_altitude': fov_data['altitude'].mean(),
                    'altitude_range': f"{fov_data['altitude'].min():.0f} - {fov_data['altitude'].max():.0f}m"
                }
                
                print(f"    Overall Error Stats:")
                print(f"      Mean ± Std: {overall_stats['mean_error']:.2f} ± {overall_stats['std_error']:.2f} meters")
                print(f"      Median: {overall_stats['median_error']:.2f} meters")
                print(f"      Range: {overall_stats['min_error']:.2f} - {overall_stats['max_error']:.2f} meters")
                print(f"      Mean Altitude: {overall_stats['mean_altitude']:.0f}m ({overall_stats['altitude_range']})")
                
                # Error as percentage of altitude
                fov_data['error_percent'] = (fov_data['abs_error'] / fov_data['altitude']) * 100
                print(f"      Error as % of altitude: {fov_data['error_percent'].mean():.4f}% ± {fov_data['error_percent'].std():.4f}%")
                
                # Statistics by altitude bins
                print(f"\n    Error by Altitude Range:")
                print(f"      {'Range':<12} {'Count':<6} {'Mean Error':<12} {'Std Error':<12} {'Error %':<10}")
                print(f"      {'-'*12} {'-'*6} {'-'*12} {'-'*12} {'-'*10}")
                
                for altitude_bin in altitude_labels:
                    bin_data = fov_data[fov_data['altitude_bin'] == altitude_bin]
                    if len(bin_data) > 0:
                        mean_err = bin_data['abs_error'].mean()
                        std_err = bin_data['abs_error'].std() if len(bin_data) > 1 else 0
                        error_pct = bin_data['error_percent'].mean()
                        print(f"      {altitude_bin:<12} {len(bin_data):<6} {mean_err:<12.2f} {std_err:<12.2f} {error_pct:<10.4f}%")
                
                # Correlation with altitude
                if len(fov_data) > 1 and fov_data['altitude'].nunique() > 1:
                    altitude_corr = fov_data['altitude'].corr(fov_data['abs_error'])
                    print(f"    Correlation with altitude: {altitude_corr:.4f}")
        
        print("\n" + "="*80)
        print("SUMMARY STATISTICS ACROSS ALL HIGH-RESOLUTION COMBINATIONS")
        print("="*80)
        
        # Overall summary for all high-res data
        print(f"Total samples: {len(high_res_data)}")
        print(f"Mean absolute error: {high_res_data['abs_error'].mean():.2f} ± {high_res_data['abs_error'].std():.2f} meters")
        print(f"Mean error as % of altitude: {(high_res_data['abs_error'] / high_res_data['altitude'] * 100).mean():.4f}%")
        
        # Best/worst performing combinations
        combo_stats = high_res_data.groupby(['resolution', 'fov_rounded']).agg({
            'abs_error': ['mean', 'std', 'count']
        }).round(4)
        combo_stats.columns = ['mean_error', 'std_error', 'count']
        combo_stats = combo_stats.reset_index()
        
        # Sort by mean error
        best_combo = combo_stats.loc[combo_stats['mean_error'].idxmin()]
        worst_combo = combo_stats.loc[combo_stats['mean_error'].idxmax()]
        
        print(f"\nBest performing combination:")
        print(f"  {best_combo['resolution']}px, {best_combo['fov_rounded']}°: {best_combo['mean_error']:.2f} ± {best_combo['std_error']:.2f}m ({best_combo['count']} samples)")
        
        print(f"\nWorst performing combination:")
        print(f"  {worst_combo['resolution']}px, {worst_combo['fov_rounded']}°: {worst_combo['mean_error']:.2f} ± {worst_combo['std_error']:.2f}m ({worst_combo['count']} samples)")
        
        print("\n" + "="*80)

    def save_processed_data(self):
        """Save the processed clean data and outlier data with additional calculated columns."""
        # Save clean data
        clean_output_path = self.csv_path.parent / f"{self.csv_path.stem}_clean.csv"
        self.df_clean.to_csv(clean_output_path, index=False)
        print(f"Clean processed data saved to {clean_output_path}")
        
        # Save extreme outliers
        if len(self.extreme_outliers) > 0:
            extreme_output_path = self.csv_path.parent / f"{self.csv_path.stem}_extreme_outliers.csv"
            self.extreme_outliers.to_csv(extreme_output_path, index=False)
            print(f"Extreme outliers saved to {extreme_output_path}")
        
        # Save complete outlier analysis
        outlier_analysis = self.analyze_outliers(95)
        if len(outlier_analysis['outlier_details']) > 0:
            outlier_output_path = self.csv_path.parent / f"{self.csv_path.stem}_all_outliers.csv"
            outlier_analysis['outlier_details'].to_csv(outlier_output_path, index=False)
            print(f"Complete outlier analysis saved to {outlier_output_path}")
        
        return clean_output_path


def main():
    """Main function to run the analysis."""
    parser = argparse.ArgumentParser(description='Analyze distance measurement errors')
    parser.add_argument('csv_file', help='Path to the CSV file containing results')
    parser.add_argument('--no-plots', action='store_true', help='Skip generating plots')
    parser.add_argument('--save-data', action='store_true', help='Save processed data')
    parser.add_argument('--output-dir', help='Directory to save outputs (default: same as input)')
    
    args = parser.parse_args()
    
    # Initialize analyzer
    analyzer = DistanceErrorAnalyzer(args.csv_file)
    
    # Generate and print summary report
    report = analyzer.generate_summary_report()
    print(report)
    
    # Print detailed altitude error statistics
    analyzer.print_altitude_error_statistics()
    
    # Save report to file
    report_path = analyzer.csv_path.parent / 'analysis_report.txt'
    with open(report_path, 'w') as f:
        f.write(report)
    print(f"\nReport saved to {report_path}")
    
    # Generate plots unless disabled
    if not args.no_plots:
        print("\nGenerating analysis plots...")
        analyzer.plot_error_vs_parameters()
        analyzer.create_correlation_matrix()
    
    # Save processed data if requested
    if args.save_data:
        analyzer.save_processed_data()
    
    print("\nAnalysis complete!")


if __name__ == "__main__":
    main()
