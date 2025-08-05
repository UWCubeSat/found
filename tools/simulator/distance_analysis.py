#!/usr/bin/env python3
"""
Distance Analysis Tool for Satellite Imaging Simulator

This tool runs 'found distance' on all generated simulation images and analyzes
the accuracy vs parameters including distance, focal length, number of pixels,
and angular deviation from nadir.

Usage:
    python distance_analysis.py <simulation_output_directory> [found_binary_path]

Example:
    python distance_analysis.py data/quick_test_20250804_120710
    python distance_analysis.py data/default_full_20250804_123456 ./build/bin/found
"""

import sys
import subprocess
from pathlib import Path
import numpy as np
import csv
import re
from datetime import datetime

# Add the src directory to path to import our modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

# Import after path setup
import __init__
from common.constants import DEFAULT_PIXEL_SIZE


def calculate_nadir_deviation(orientation_str: str) -> float:
    """
    Calculate angular deviation from nadir (straight down) given orientation string.
    
    Args:
        orientation_str: String like "0.00 9.92 0" representing RA, Dec, Roll
    
    Returns:
        Angular deviation from nadir in degrees
    """
    try:
        parts = orientation_str.split()
        ra, dec, roll = float(parts[0]), float(parts[1]), float(parts[2])
        
        # Nadir is straight down: RA=0, Dec=0, Roll=0
        # Calculate angular distance from nadir point
        nadir_deviation = np.sqrt(ra**2 + dec**2)
        return nadir_deviation
    except:
        return 0.0


def convert_degrees_to_radians_safe(orientation_str: str) -> str:
    """
    Convert orientation from degrees to radians and ensure valid ranges.
    
    Args:
        orientation_str: String like "45.0 -30.0 0.0" in degrees
        
    Returns:
        String like "0.785 0.524 0.0" in radians with safe values
    """
    try:
        parts = orientation_str.split()
        if len(parts) != 3:
            print(f"   🐛 DEBUG: Invalid orientation format: {orientation_str}, using default")
            return "0.0 0.0 0.0"
        
        ra_deg, dec_deg, roll_deg = float(parts[0]), float(parts[1]), float(parts[2])
        
        # Handle negative zero and negative RA values
        if ra_deg <= 0:  # This catches both negative values and -0
            if ra_deg < 0:
                ra_deg = ra_deg + 360
                print(f"   🐛 DEBUG: Converted negative RA {float(parts[0])}° to {ra_deg}°")
            else:  # Handle -0 case
                ra_deg = 0.0
                print(f"   🐛 DEBUG: Converted -0 RA to 0.0°")
        
        # Handle negative Dec values - convert to positive equivalent
        # Dec should be between -90 and 90, but we'll convert negatives for safety
        if dec_deg < 0:
            dec_deg = abs(dec_deg)  # Make positive
            print(f"   🐛 DEBUG: Converted negative Dec {float(parts[1])}° to {dec_deg}°")
        
        # Clamp to valid ranges - ensure all positive
        ra_deg = max(0.0, min(360.0, ra_deg))
        dec_deg = max(0.0, min(90.0, dec_deg))  # Force Dec to 0-90 range
        roll_deg = abs(roll_deg)  # Make roll positive too
        roll_deg = max(0.0, min(180.0, roll_deg))  # Clamp roll to 0-180
        
        # Handle negative zero for all values to ensure clean output
        if ra_deg == 0.0:
            ra_deg = 0.0  # Force positive zero
        if dec_deg == 0.0:
            dec_deg = 0.0  # Force positive zero  
        if roll_deg == 0.0:
            roll_deg = 0.0  # Force positive zero
        
        # Convert to radians
        ra_rad = ra_deg * np.pi / 180.0
        dec_rad = dec_deg * np.pi / 180.0  
        roll_rad = roll_deg * np.pi / 180.0
        
        # Ensure no negative zeros in radians either
        if ra_rad == 0.0:
            ra_rad = 0.0
        if dec_rad == 0.0:
            dec_rad = 0.0
        if roll_rad == 0.0:
            roll_rad = 0.0
        
        # Double-check: ensure all radians are positive
        ra_rad = abs(ra_rad)
        dec_rad = abs(dec_rad)
        roll_rad = abs(roll_rad)
        
        result = f"{ra_rad:.6f} {dec_rad:.6f} {roll_rad:.6f}"
        print(f"   🐛 DEBUG: Converted orientation: {orientation_str}° → {result} rad")
        print(f"   🐛 DEBUG: Degree values used: RA={ra_deg}°, Dec={dec_deg}°, Roll={roll_deg}°")
        print(f"   🐛 DEBUG: Final radian values: RA={ra_rad:.6f}, Dec={dec_rad:.6f}, Roll={roll_rad:.6f}")
        
        return result
        
    except Exception as e:
        print(f"   🐛 DEBUG: Error converting orientation {orientation_str}: {e}, using default")
        return "0.0 0.0 0.0"


def parse_found_distance_output(output_text: str) -> float:
    """
    Parse the output from 'found distance' command to extract the measured distance.
    
    Args:
        output_text: The stdout from the found distance command
        
    Returns:
        Measured distance in meters, or None if parsing failed
    """
    print(f"   🐛 DEBUG: Parsing output for distance...")
    print(f"   🐛 DEBUG: Full output text:")
    print(f"   🐛 DEBUG: {'-'*50}")
    for i, line in enumerate(output_text.strip().split('\n')):
        print(f"   🐛 DEBUG: Line {i+1}: {repr(line)}")
    print(f"   🐛 DEBUG: {'-'*50}")
    
    try:
        lines = output_text.strip().split('\n')
        for line_num, line in enumerate(lines, 1):
            line_lower = line.lower()
            print(f"   🐛 DEBUG: Checking line {line_num}: {repr(line)}")
            
            # Look for position vector output pattern
            if 'calculated position:' in line_lower:
                print(f"   🐛 DEBUG: Found position pattern in line {line_num}")
                
                # Extract the position vector from parentheses
                # Format: "Calculated Position: (-5.06396e+06, 3.38295e+06, 2.95309e+06) m"
                import re
                vector_match = re.search(r'\(([^)]+)\)', line)
                if vector_match:
                    vector_str = vector_match.group(1)
                    print(f"   🐛 DEBUG: Position vector string: {repr(vector_str)}")
                    
                    # Split by comma and parse coordinates
                    coords = [float(x.strip()) for x in vector_str.split(',')]
                    if len(coords) == 3:
                        x, y, z = coords
                        print(f"   🐛 DEBUG: Position coordinates: x={x}, y={y}, z={z}")
                        
                        # Calculate magnitude (distance from origin)
                        distance = np.sqrt(x**2 + y**2 + z**2)
                        print(f"   🐛 DEBUG: Calculated distance magnitude: {distance} meters")
                        return distance
            
            # Look for distance output patterns (original patterns)
            elif 'distance:' in line_lower or 'estimated distance:' in line_lower:
                print(f"   🐛 DEBUG: Found distance pattern in line {line_num}")
                # Extract number from line like "Distance: 12345.67 meters" or "Distance: 12345.67"
                parts = line.split(':')
                if len(parts) >= 2:
                    distance_part = parts[-1].strip()
                    print(f"   🐛 DEBUG: Distance part: {repr(distance_part)}")
                    # Extract first number from the string
                    numbers = re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', distance_part)
                    print(f"   🐛 DEBUG: Found numbers: {numbers}")
                    if numbers:
                        distance = float(numbers[0])
                        print(f"   🐛 DEBUG: Parsed distance: {distance} meters")
                        return distance
            
            # Alternative patterns
            elif 'result:' in line_lower and any(unit in line_lower for unit in ['m', 'meter', 'km']):
                print(f"   🐛 DEBUG: Found result pattern in line {line_num}")
                numbers = re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', line)
                print(f"   🐛 DEBUG: Found numbers in result: {numbers}")
                if numbers:
                    distance = float(numbers[0])
                    # Convert km to meters if needed
                    if 'km' in line_lower:
                        distance *= 1000
                        print(f"   🐛 DEBUG: Converted km to meters: {distance}")
                    print(f"   🐛 DEBUG: Parsed distance from result: {distance} meters")
                    return distance
        
        # If no specific pattern found, try to extract any large number that could be distance
        print(f"   🐛 DEBUG: No specific pattern found, looking for large numbers...")
        all_numbers = re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', output_text)
        print(f"   🐛 DEBUG: All numbers found: {all_numbers}")
        large_numbers = [float(n) for n in all_numbers if float(n) > 1000]  # Distance should be > 1km
        print(f"   🐛 DEBUG: Large numbers (>1000): {large_numbers}")
        
        if large_numbers:
            distance = large_numbers[0]
            print(f"   🐛 DEBUG: Using first large number as distance: {distance} meters")
            return distance
            
    except Exception as e:
        print(f"   🐛 DEBUG: Parse error: {e}")
    
    print(f"   🐛 DEBUG: Could not parse any distance from output")
    return None


def run_found_distance_analysis(output_dir: Path, distance_binary_path: str = "./build/bin/found"):
    """
    Run 'found distance' on all images and analyze accuracy vs parameters.
    
    Args:
        output_dir: Path to simulation output directory
        distance_binary_path: Path to the found binary
    """
    print(f"🔬 Running Distance Analysis on: {output_dir}")
    print("=" * 60)
    
    metadata_file = output_dir / "image_commands.txt"
    if not metadata_file.exists():
        print(f"❌ Metadata file not found: {metadata_file}")
        print("   Make sure this directory contains simulation results with metadata.")
        return
    
    # Check if found binary exists
    found_binary = Path(distance_binary_path)
    if not found_binary.exists():
        print(f"❌ Found binary not found: {found_binary}")
        print("   Please build the project or provide correct path to the 'found' executable.")
        return
    
    print(f"✅ Using found binary: {found_binary}")
    print(f"✅ Reading metadata from: {metadata_file}")
    
    # Results storage
    results = []
    total_processed = 0
    successful_measurements = 0
    
    try:
        with open(metadata_file, 'r') as f:
            lines = f.readlines()
        
        # Filter out comments and empty lines
        data_lines = [line.strip() for line in lines if line.strip() and not line.startswith('#')]
        
        print(f"📊 Found {len(data_lines)} images to process")
        
        for line_num, line in enumerate(data_lines, 1):
            if ' | ' not in line:
                continue
            
            file_name, command = line.split(' | ', 1)
            image_path = output_dir / file_name.strip()
            
            if not image_path.exists():
                print(f"⚠️  Image not found: {image_path}")
                continue
            
            total_processed += 1
            print(f"📸 Processing {file_name.strip()} ({line_num}/{len(data_lines)})...")
            print(f"   🐛 DEBUG: Original command: {command}")
            
            # Parse simulation parameters from command
            cmd_parts = command.split()
            params = {}
            
            try:
                # Extract parameters
                if '--position' in cmd_parts:
                    pos_idx = cmd_parts.index('--position')
                    params['true_distance'] = abs(float(cmd_parts[pos_idx + 1]))  # Distance from Earth center
                    print(f"   🐛 DEBUG: True distance: {params['true_distance']} meters ({params['true_distance']/1000:.1f} km)")
                
                if '--orientation' in cmd_parts:
                    ori_idx = cmd_parts.index('--orientation')
                    orientation_str = f"{cmd_parts[ori_idx + 1]} {cmd_parts[ori_idx + 2]} {cmd_parts[ori_idx + 3]}"
                    print(f"   🐛 DEBUG: Original orientation (degrees): {orientation_str}")
                    
                    # Convert to radians and make safe
                    safe_orientation = convert_degrees_to_radians_safe(orientation_str)
                    params['orientation'] = safe_orientation
                    params['nadir_deviation'] = calculate_nadir_deviation(orientation_str)
                    print(f"   🐛 DEBUG: Safe orientation (radians): {safe_orientation}")
                    print(f"   🐛 DEBUG: Nadir deviation: {params['nadir_deviation']:.2f}°")
                else:
                    params['orientation'] = "0.0 0.0 0.0"
                    params['nadir_deviation'] = 0.0
                    print(f"   🐛 DEBUG: No orientation found, using default: {params['orientation']}")
                
                if '--focal-length' in cmd_parts:
                    focal_idx = cmd_parts.index('--focal-length')
                    params['focal_length'] = float(cmd_parts[focal_idx + 1])
                    print(f"   🐛 DEBUG: Focal length: {params['focal_length']} m ({params['focal_length']*1000:.1f} mm)")
                else:
                    params['focal_length'] = 0.01
                    print(f"   🐛 DEBUG: No focal length found, using default: {params['focal_length']} m")
                
                if '--pixel-size' in cmd_parts:
                    pixel_idx = cmd_parts.index('--pixel-size')
                    params['pixel_size'] = float(cmd_parts[pixel_idx + 1])
                    print(f"   🐛 DEBUG: Pixel size: {params['pixel_size']} m ({params['pixel_size']*1e6:.1f} μm)")
                else:
                    params['pixel_size'] = DEFAULT_PIXEL_SIZE
                    print(f"   🐛 DEBUG: No pixel size found, using default: {params['pixel_size']} m")
                
                if '--x-resolution' in cmd_parts:
                    res_idx = cmd_parts.index('--x-resolution')
                    params['resolution'] = int(cmd_parts[res_idx + 1])
                    params['num_pixels'] = params['resolution'] ** 2
                    print(f"   🐛 DEBUG: Resolution: {params['resolution']}x{params['resolution']} ({params['num_pixels']:,} pixels)")
                else:
                    params['resolution'] = 512
                    params['num_pixels'] = 512 ** 2
                    print(f"   🐛 DEBUG: No resolution found, using default: {params['resolution']}x{params['resolution']}")
            
            except (ValueError, IndexError) as e:
                print(f"   ❌ Failed to parse parameters: {e}")
                continue
            
            # Run found distance command
            distance_cmd = [
                str(found_binary), "distance",
                "--image", str(image_path),
                "--reference-as-orientation",
                "--camera-focal-length", str(params.get('focal_length', 0.01)),
                "--camera-pixel-size", str(params.get('pixel_size', DEFAULT_PIXEL_SIZE)),
                "--reference-orientation", *params.get('orientation', '0.0 0.0 0.0').split()
            ]
            
            print(f"   🐛 DEBUG: Full command to execute:")
            print(f"   🐛 DEBUG: {' '.join(distance_cmd)}")
            print(f"   🐛 DEBUG: Working directory: {Path(__file__).parent.parent.parent}")
            
            try:
                result = subprocess.run(
                    distance_cmd,
                    cwd=Path(__file__).parent.parent.parent,  # Go to project root
                    capture_output=True,
                    text=True,
                    timeout=30
                )
                
                print(f"   🐛 DEBUG: Command return code: {result.returncode}")
                
                if result.stdout:
                    print(f"   🐛 DEBUG: Command stdout:")
                    for i, line in enumerate(result.stdout.strip().split('\n')):
                        print(f"   🐛 DEBUG: STDOUT Line {i+1}: {repr(line)}")
                
                if result.stderr:
                    print(f"   🐛 DEBUG: Command stderr:")
                    for i, line in enumerate(result.stderr.strip().split('\n')):
                        print(f"   🐛 DEBUG: STDERR Line {i+1}: {repr(line)}")
                
                if result.returncode == 0:
                    # Parse distance result
                    measured_distance = parse_found_distance_output(result.stdout)
                    
                    if measured_distance is not None:
                        true_dist = params['true_distance']
                        error = measured_distance - true_dist
                        relative_error = (error / true_dist) * 100
                        
                        print(f"   🐛 DEBUG: Distance calculation:")
                        print(f"   🐛 DEBUG:   True distance: {true_dist} meters ({true_dist/1000:.3f} km)")
                        print(f"   🐛 DEBUG:   Measured distance: {measured_distance} meters ({measured_distance/1000:.3f} km)")
                        print(f"   🐛 DEBUG:   Absolute error: {error} meters ({error/1000:.3f} km)")
                        print(f"   🐛 DEBUG:   Relative error: {relative_error:.3f}%")
                        
                        params.update({
                            'measured_distance': measured_distance,
                            'error': error,
                            'relative_error': relative_error,
                            'image_file': file_name.strip(),
                            'found_output': result.stdout.strip()
                        })
                        
                        results.append(params)
                        successful_measurements += 1
                        
                        print(f"   ✅ True: {true_dist/1000:.1f}km, Measured: {measured_distance/1000:.1f}km, Error: {relative_error:.2f}%")
                    else:
                        print(f"   ❌ Could not parse distance from output")
                else:
                    print(f"   ❌ Distance command failed (code {result.returncode})")
            
            except subprocess.TimeoutExpired:
                print(f"   ⏰ Distance analysis timeout")
            except Exception as e:
                print(f"   ❌ Error running distance analysis: {e}")
    
    except Exception as e:
        print(f"❌ Error reading metadata: {e}")
        return
    
    # Analysis and correlation
    print(f"\n📊 Analysis Results")
    print("=" * 60)
    print(f"   Total images processed: {total_processed}")
    print(f"   Successful measurements: {successful_measurements}")
    if total_processed > 0:
        print(f"   Success rate: {(successful_measurements/total_processed)*100:.1f}%")
    
    if not results:
        print("❌ No successful distance measurements")
        return
    
    # Statistical summary
    errors = [r['relative_error'] for r in results]
    nadir_devs = [r['nadir_deviation'] for r in results]
    focal_lengths = [r['focal_length'] for r in results]
    distances = [r['true_distance'] for r in results]
    num_pixels = [r['num_pixels'] for r in results]
    
    print(f"\n📈 Error Statistics:")
    print(f"   Mean relative error: {np.mean(errors):.2f}%")
    print(f"   Std deviation: {np.std(errors):.2f}%")
    print(f"   Min error: {np.min(errors):.2f}%")
    print(f"   Max error: {np.max(errors):.2f}%")
    print(f"   Median error: {np.median(errors):.2f}%")
    
    print(f"\n📐 Parameter Ranges:")
    print(f"   Nadir deviation: {np.min(nadir_devs):.1f}° to {np.max(nadir_devs):.1f}°")
    print(f"   Focal length: {np.min(focal_lengths)*1000:.1f}mm to {np.max(focal_lengths)*1000:.1f}mm")
    print(f"   True distance: {np.min(distances)/1000:.1f}km to {np.max(distances)/1000:.1f}km")
    print(f"   Num pixels: {np.min(num_pixels):,} to {np.max(num_pixels):,}")
    
    # Correlation analysis
    print(f"\n🔗 Correlation Analysis:")
    if len(set(nadir_devs)) > 1:  # Check for variance
        correlations = {
            'Nadir Deviation vs Error': np.corrcoef(nadir_devs, [abs(e) for e in errors])[0,1],
            'Focal Length vs Error': np.corrcoef(focal_lengths, [abs(e) for e in errors])[0,1] if len(set(focal_lengths)) > 1 else 0.0,
            'Distance vs Error': np.corrcoef(distances, [abs(e) for e in errors])[0,1] if len(set(distances)) > 1 else 0.0,
            'Num Pixels vs Error': np.corrcoef(num_pixels, [abs(e) for e in errors])[0,1] if len(set(num_pixels)) > 1 else 0.0
        }
        
        for param, corr in correlations.items():
            if not np.isnan(corr):
                strength = "Strong" if abs(corr) > 0.7 else "Moderate" if abs(corr) > 0.3 else "Weak"
                direction = "positive" if corr > 0 else "negative"
                print(f"   {param}: {corr:.3f} ({strength} {direction})")
            else:
                print(f"   {param}: N/A (insufficient variance)")
    else:
        print("   Insufficient parameter variance for correlation analysis")
    
    # Save detailed results
    results_file = output_dir / "distance_analysis_results.csv"
    try:
        with open(results_file, 'w', newline='') as csvfile:
            if results:
                # Define field order for better readability
                fieldnames = [
                    'image_file', 'true_distance', 'measured_distance', 'error', 'relative_error',
                    'nadir_deviation', 'focal_length', 'pixel_size', 'resolution', 'num_pixels',
                    'orientation'
                ]
                
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames, extrasaction='ignore')
                writer.writeheader()
                writer.writerows(results)
        print(f"\n💾 Detailed results saved to: {results_file}")
    except Exception as e:
        print(f"⚠️  Could not save CSV results: {e}")
    
    # Performance analysis
    if results:
        print(f"\n🎯 Best Performing Images (Lowest Error):")
        sorted_results = sorted(results, key=lambda x: abs(x['relative_error']))
        for i, result in enumerate(sorted_results[:5]):
            print(f"   #{i+1}: {result['image_file']}")
            print(f"       Error: {result['relative_error']:.2f}%, Nadir deviation: {result['nadir_deviation']:.1f}°")
            print(f"       Distance: {result['true_distance']/1000:.1f}km, Focal: {result['focal_length']*1000:.1f}mm")
        
        print(f"\n📉 Worst Performing Images (Highest Error):")
        for i, result in enumerate(sorted_results[-5:][::-1], 1):
            print(f"   #{i}: {result['image_file']}")
            print(f"       Error: {result['relative_error']:.2f}%, Nadir deviation: {result['nadir_deviation']:.1f}°")
            print(f"       Distance: {result['true_distance']/1000:.1f}km, Focal: {result['focal_length']*1000:.1f}mm")
    
    # Recommendations
    print(f"\n💡 Analysis Insights:")
    
    # Find optimal nadir deviation range
    if len(set(nadir_devs)) > 1:
        low_error_results = [r for r in results if abs(r['relative_error']) < np.median([abs(e) for e in errors])]
        if low_error_results:
            optimal_nadir_devs = [r['nadir_deviation'] for r in low_error_results]
            print(f"   • Optimal nadir deviation range: {np.min(optimal_nadir_devs):.1f}° - {np.max(optimal_nadir_devs):.1f}°")
            print(f"   • Best performing nadir deviation: {sorted_results[0]['nadir_deviation']:.1f}°")
    
    # Distance vs error insights
    if len(set(distances)) > 1:
        distance_corr = np.corrcoef(distances, [abs(e) for e in errors])[0,1]
        if not np.isnan(distance_corr):
            if distance_corr > 0.3:
                print(f"   • Accuracy decreases with distance (correlation: {distance_corr:.3f})")
            elif distance_corr < -0.3:
                print(f"   • Accuracy improves with distance (correlation: {distance_corr:.3f})")
    
    print(f"\n✅ Distance analysis complete!")
    return results


def main():
    """Main function for the distance analysis tool."""
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python distance_analysis.py <simulation_output_directory> [found_binary_path]")
        print()
        print("Examples:")
        print("  python distance_analysis.py data/quick_test_20250804_120710")
        print("  python distance_analysis.py data/default_full_20250804_123456 ./build/bin/found")
        print("  python distance_analysis.py /tmp/simulator_output_quick_test_20250804_115011")
        return
    
    output_dir = Path(sys.argv[1])
    distance_binary_path = sys.argv[2] if len(sys.argv) > 2 else "./build/bin/found"
    
    if not output_dir.exists():
        print(f"❌ Output directory not found: {output_dir}")
        return
    
    if not output_dir.is_dir():
        print(f"❌ Path is not a directory: {output_dir}")
        return
    
    print(f"🔍 Starting distance analysis...")
    print(f"   Output directory: {output_dir}")
    print(f"   Found binary: {distance_binary_path}")
    print()
    
    try:
        results = run_found_distance_analysis(output_dir, distance_binary_path)
        
        if results:
            print(f"\n🎉 Analysis completed successfully!")
            print(f"   Results saved in: {output_dir}/distance_analysis_results.csv")
        else:
            print(f"\n⚠️  Analysis completed but no successful measurements were obtained.")
            
    except KeyboardInterrupt:
        print("\n👋 Analysis interrupted by user.")
    except Exception as e:
        print(f"\n❌ Analysis failed: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
