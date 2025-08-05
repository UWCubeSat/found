#!/usr/bin/env python3
"""
Quick analysis runner for distance error analysis.

This script provides a s        # FOV performance summary (using rounded values)
        print(f"\nFOV performance summary (rounded to nearest degree):")
        if df['fov_rounded'].nunique() <= 1:
            print(f"  All samples have same FOV: {df['fov_rounded'].iloc[0]:.0f}° (no variance to analyze)")
        else:
            fov_summary = df.groupby('fov_rounded')['abs_relative_error'].agg(['mean', 'std', 'count'])
            for fov, stats in fov_summary.iterrows():
                print(f"  {fov:>3.0f}°: mean={stats['mean']:.4f}, std={stats['std']:.4f}, samples={stats['count']}") way to run the distance error analysis
on the CSV results file.
"""

from distance_error_analysis import DistanceErrorAnalyzer
import sys
from pathlib import Path

def run_analysis(csv_path: str):
    """Run the complete analysis on the CSV file."""
    try:
        # Initialize the analyzer
        print("Initializing Distance Error Analyzer...")
        analyzer = DistanceErrorAnalyzer(csv_path)
        
        # Generate summary report
        print("\nGenerating summary report...")
        report = analyzer.generate_summary_report()
        print(report)
        
        # Save report
        report_path = analyzer.csv_path.parent / 'analysis_report.txt'
        with open(report_path, 'w') as f:
            f.write(report)
        print(f"\nReport saved to: {report_path}")
        
        # Generate visualizations
        print("\nGenerating analysis plots...")
        analyzer.plot_error_vs_parameters()
        
        print("\nGenerating correlation matrix...")
        corr_matrix = analyzer.create_correlation_matrix()
        
        # Save processed data
        print("\nSaving processed data...")
        processed_path = analyzer.save_processed_data()
        
        print(f"\nAnalysis complete! Files saved to: {analyzer.csv_path.parent}")
        
        return analyzer
        
    except Exception as e:
        print(f"Error during analysis: {e}")
        return None

def main():
    """Main function."""
    # Default path to the CSV file
    default_csv = "/Users/joshualando/Documents/GitHub/found/tools/simulator/data/default_full_20250805_061049/distance_analysis_results.csv"
    
    # Use command line argument if provided, otherwise use default
    csv_path = sys.argv[1] if len(sys.argv) > 1 else default_csv
    
    if not Path(csv_path).exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Usage: python run_analysis.py [path_to_csv_file]")
        return
    
    print(f"Running analysis on: {csv_path}")
    analyzer = run_analysis(csv_path)
    
    if analyzer:
        print("\n" + "="*60)
        print("QUICK INSIGHTS:")
        print("="*60)
        
        # Print some quick insights from clean data
        df = analyzer.df_clean
        df_original = analyzer.df
        extreme_outliers = analyzer.extreme_outliers
        
        print(f"Original samples: {len(df_original)}")
        print(f"Clean samples (after removing 3σ outliers): {len(df)}")
        print(f"Extreme outliers removed: {len(extreme_outliers)}")
        print(f"Average absolute relative error (clean): {df['abs_relative_error'].mean():.4f}")
        print(f"Error standard deviation (clean): {df['abs_relative_error'].std():.4f}")
        
        # Best and worst configurations from clean data
        best_idx = df['abs_relative_error'].idxmin()
        worst_idx = df['abs_relative_error'].idxmax()
        
        print(f"\nBest configuration (from clean data):")
        print(f"  File: {df.loc[best_idx, 'image_file']}")
        print(f"  Resolution: {df.loc[best_idx, 'resolution']}px")
        print(f"  FOV: {df.loc[best_idx, 'fov_rounded']:.0f}°")
        print(f"  Error: {df.loc[best_idx, 'abs_relative_error']:.4f}")
        
        print(f"\nWorst configuration (from clean data):")
        print(f"  File: {df.loc[worst_idx, 'image_file']}")
        print(f"  Resolution: {df.loc[worst_idx, 'resolution']}px")
        print(f"  FOV: {df.loc[worst_idx, 'fov_rounded']:.0f}°")
        print(f"  Error: {df.loc[worst_idx, 'abs_relative_error']:.4f}")
        
        # Resolution performance summary from clean data
        print(f"\nResolution performance summary (clean data):")
        res_summary = df.groupby('resolution')['abs_relative_error'].agg(['mean', 'std', 'count'])
        for res, stats in res_summary.iterrows():
            print(f"  {res:>4}px: mean={stats['mean']:.4f}, std={stats['std']:.4f}, samples={stats['count']}")
        
        # FOV performance summary (using rounded values)
        print(f"\nFOV performance summary (clean data, rounded to nearest degree):")
        fov_summary = df.groupby('fov_rounded')['abs_relative_error'].agg(['mean', 'std', 'count'])
        for fov, stats in fov_summary.iterrows():
            print(f"  {fov:>3.0f}°: mean={stats['mean']:.4f}, std={stats['std']:.4f}, samples={stats['count']}")
        
        # Generate outlier report
        print(f"\n" + "="*60)
        print("OUTLIER ANALYSIS:")
        print("="*60)
        outlier_analysis = analyzer.analyze_outliers(95)
        outliers_df = outlier_analysis['outlier_details']
        
        print(f"Extreme outliers (>3σ): {outlier_analysis['extreme_outliers']}")
        print(f"Clean data outliers (top 5%): {outlier_analysis['clean_outliers']}")
        print(f"Total outliers: {outlier_analysis['total_outliers']}")
        
        if outlier_analysis['total_outliers'] > 0:
            print(f"Worst error overall: {outlier_analysis['worst_error']:.4f}")
            
            # Show extreme outliers first
            if len(extreme_outliers) > 0:
                print(f"\nExtreme outliers (>3σ):")
                for i, (idx, row) in enumerate(extreme_outliers.head(3).iterrows()):
                    print(f"\n{i+1}. {row['image_file']}")
                    print(f"   Error: {row['abs_relative_error']:.4f}")
                    print(f"   Resolution: {row['resolution']}px, FOV: {row['fov_rounded']:.0f}°")
                    print(f"   Generation command:")
                    print(f"   {row['generation_command']}")
            
            # Show clean data outliers
            clean_outliers = outliers_df[outliers_df['outlier_type'] == 'percentile_based']
            if len(clean_outliers) > 0:
                print(f"\nTop clean data outliers (from remaining data):")
                for i, (idx, row) in enumerate(clean_outliers.head(3).iterrows()):
                    print(f"\n{i+1}. {row['image_file']}")
                    print(f"   Error: {row['abs_relative_error']:.4f}")
                    print(f"   Resolution: {row['resolution']}px, FOV: {row['fov_rounded']:.0f}°")
                    print(f"   Generation command:")
                    print(f"   {row['generation_command']}")
        
        # Save outlier details to CSV
        if len(outliers_df) > 0:
            outlier_path = analyzer.csv_path.parent / 'complete_outlier_analysis.csv'
            outliers_df.to_csv(outlier_path, index=False)
            print(f"\nComplete outlier analysis saved to: {outlier_path}")

if __name__ == "__main__":
    main()
