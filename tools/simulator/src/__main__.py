#!/usr/bin/env python3
"""
Main entry point for the satellite imaging simulator.

This script runs a default simulation using parameters from common.constants.py
and functions from parameters.py and utility.py. It generates images using the
generator tool and stores them in an appropriately named output folder.
"""

import sys
import os
import subprocess
from pathlib import Path
from datetime import datetime
import itertools

# Import the __init__ module to trigger path manipulation FIRST
import __init__

# NOW we can import numpy and other modules
import numpy as np

# Import simulator modules after path setup
from simulate.parameters import generate_command_flags
from common.constants import (
    SIMULATION_RANGES, 
    DEFAULT_SIMULATION, 
    DEFAULT_PIXEL_SIZE,
)
from common.utility import focal_length_from_fov_and_resolution


def lookup_image_command(output_dir: Path, image_name: str) -> str:
    """
    Look up the command used to generate a specific image.
    
    Args:
        output_dir: Path to the simulation output directory
        image_name: Name of the image file (with or without .png extension)
    
    Returns:
        The full command string used to generate the image, or None if not found
    """
    metadata_file = output_dir / "image_commands.txt"
    
    if not metadata_file.exists():
        print(f"❌ Metadata file not found: {metadata_file}")
        return None
    
    # Normalize image name (ensure .png extension)
    if not image_name.endswith('.png'):
        image_name += '.png'
    
    try:
        with open(metadata_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line.startswith('#') or not line:
                    continue
                
                if ' | ' in line:
                    file_name, command = line.split(' | ', 1)
                    if file_name.strip() == image_name:
                        return command.strip()
        
        print(f"❌ Image '{image_name}' not found in metadata")
        return None
        
    except Exception as e:
        print(f"❌ Error reading metadata file: {e}")
        return None


def reproduce_image(output_dir: Path, image_name: str, new_output_path: str = None):
    """
    Reproduce a specific image by looking up and re-running its original command.
    
    Args:
        output_dir: Path to the original simulation output directory
        image_name: Name of the image to reproduce
        new_output_path: Optional new output path (defaults to current directory)
    """
    command = lookup_image_command(output_dir, image_name)
    
    if not command:
        return False
    
    print(f"🔍 Found command for {image_name}:")
    print(f"   {command}")
    
    if new_output_path:
        # Replace the filename in the command
        cmd_parts = command.split()
        try:
            filename_idx = cmd_parts.index('--filename') + 1
            cmd_parts[filename_idx] = new_output_path
            command = ' '.join(cmd_parts)
            print(f"📝 Modified output path to: {new_output_path}")
        except (ValueError, IndexError):
            print("⚠️  Could not modify output path in command")
    
    print("🚀 Re-running command...")
    
    try:
        result = subprocess.run(
            command.split(),
            cwd=Path(__file__).parent.parent.parent.parent,
            capture_output=True,
            text=True,
            timeout=60
        )
        
        if result.returncode == 0:
            print("✅ Image reproduced successfully!")
            return True
        else:
            print("❌ Command failed:")
            print(f"   {result.stderr}")
            return False
            
    except Exception as e:
        print(f"❌ Error running command: {e}")
        return False


def analyze_simulation_results(output_dir: Path):
    """
    Analyze and summarize simulation results from a metadata file.
    
    Args:
        output_dir: Path to the simulation output directory
    """
    metadata_file = output_dir / "image_commands.txt"
    
    if not metadata_file.exists():
        print(f"❌ Metadata file not found: {metadata_file}")
        return
    
    print(f"📊 Analyzing simulation results in: {output_dir}")
    print("=" * 50)
    
    total_images = 0
    unique_distances = set()
    unique_fovs = set()
    unique_resolutions = set()
    
    try:
        with open(metadata_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line.startswith('#') or not line:
                    continue
                
                if ' | ' in line:
                    file_name, command = line.split(' | ', 1)
                    total_images += 1
                    
                    # Parse command for parameters
                    cmd_parts = command.split()
                    
                    # Extract key parameters
                    try:
                        if '--position' in cmd_parts:
                            pos_idx = cmd_parts.index('--position')
                            distance = float(cmd_parts[pos_idx + 1])
                            unique_distances.add(distance)
                        
                        if '--x-resolution' in cmd_parts:
                            res_idx = cmd_parts.index('--x-resolution')
                            resolution = int(cmd_parts[res_idx + 1])
                            unique_resolutions.add(resolution)
                            
                        if '--focal-length' in cmd_parts:
                            focal_idx = cmd_parts.index('--focal-length')
                            focal_length = float(cmd_parts[focal_idx + 1])
                            # Calculate FOV from focal length and resolution (approximate)
                            if '--pixel-size' in cmd_parts:
                                pixel_idx = cmd_parts.index('--pixel-size')
                                pixel_size = float(cmd_parts[pixel_idx + 1])
                                sensor_width = resolution * pixel_size
                                fov_rad = 2 * np.arctan(sensor_width / (2 * focal_length))
                                fov_deg = np.degrees(fov_rad)
                                unique_fovs.add(round(fov_deg, 1))
                    except (ValueError, IndexError):
                        pass  # Skip parsing errors
    
        print(f"📈 Summary:")
        print(f"   Total images: {total_images}")
        print(f"   Unique distances: {len(unique_distances)} ({sorted([d/1_000_000 for d in unique_distances])} Mm)")
        print(f"   Unique FOVs: {len(unique_fovs)} ({sorted(unique_fovs)}°)")
        print(f"   Unique resolutions: {len(unique_resolutions)} ({sorted(unique_resolutions)} px)")
        
        print(f"\n📁 Files in directory:")
        png_files = list(output_dir.glob("*.png"))
        print(f"   PNG files: {len(png_files)}")
        print(f"   Metadata file: {metadata_file.name}")
        
    except Exception as e:
        print(f"❌ Error analyzing results: {e}")


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
        return
    
    # Results storage
    results = []
    
    try:
        with open(metadata_file, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if line.startswith('#') or not line:
                    continue
                
                if ' | ' not in line:
                    continue
                
                file_name, command = line.split(' | ', 1)
                image_path = output_dir / file_name.strip()
                
                if not image_path.exists():
                    print(f"⚠️  Image not found: {image_path}")
                    continue
                
                print(f"📸 Processing {file_name.strip()} ({line_num})...")
                
                # Parse simulation parameters from command
                cmd_parts = command.split()
                params = {}
                
                try:
                    # Extract parameters
                    if '--position' in cmd_parts:
                        pos_idx = cmd_parts.index('--position')
                        params['true_distance'] = abs(float(cmd_parts[pos_idx + 1]))  # Distance from Earth center
                    
                    if '--orientation' in cmd_parts:
                        ori_idx = cmd_parts.index('--orientation')
                        orientation_str = f"{cmd_parts[ori_idx + 1]} {cmd_parts[ori_idx + 2]} {cmd_parts[ori_idx + 3]}"
                        params['orientation'] = orientation_str
                        params['nadir_deviation'] = calculate_nadir_deviation(orientation_str)
                    
                    if '--focal-length' in cmd_parts:
                        focal_idx = cmd_parts.index('--focal-length')
                        params['focal_length'] = float(cmd_parts[focal_idx + 1])
                    
                    if '--pixel-size' in cmd_parts:
                        pixel_idx = cmd_parts.index('--pixel-size')
                        params['pixel_size'] = float(cmd_parts[pixel_idx + 1])
                    
                    if '--x-resolution' in cmd_parts:
                        res_idx = cmd_parts.index('--x-resolution')
                        params['resolution'] = int(cmd_parts[res_idx + 1])
                        params['num_pixels'] = params['resolution'] ** 2
                
                except (ValueError, IndexError) as e:
                    print(f"   ❌ Failed to parse parameters: {e}")
                    continue
                
                # Run found distance command
                distance_cmd = [
                    distance_binary_path, "distance",
                    "--image", str(image_path),
                    "--reference-as-orientation",
                    "--camera-focal-length", str(params.get('focal_length', 0.01)),
                    "--camera-pixel-size", str(params.get('pixel_size', 20e-6)),
                    "--reference-orientation", *params.get('orientation', '0 0 0').split()
                ]
                
                try:
                    result = subprocess.run(
                        distance_cmd,
                        cwd=Path(__file__).parent.parent.parent.parent,
                        capture_output=True,
                        text=True,
                        timeout=30
                    )
                    
                    if result.returncode == 0:
                        # Parse distance result
                        output_lines = result.stdout.strip().split('\n')
                        measured_distance = None
                        
                        for line in output_lines:
                            if 'distance:' in line.lower():
                                try:
                                    # Extract distance value (assuming format like "Distance: 12345.67 meters")
                                    distance_str = line.split(':')[-1].strip()
                                    measured_distance = float(distance_str.split()[0])
                                    break
                                except:
                                    continue
                        
                        if measured_distance is not None:
                            true_dist = params['true_distance']
                            error = measured_distance - true_dist
                            relative_error = (error / true_dist) * 100
                            
                            params.update({
                                'measured_distance': measured_distance,
                                'error': error,
                                'relative_error': relative_error,
                                'image_file': file_name.strip()
                            })
                            
                            results.append(params)
                            
                            print(f"   ✅ True: {true_dist/1000:.1f}km, Measured: {measured_distance/1000:.1f}km, Error: {relative_error:.2f}%")
                        else:
                            print(f"   ❌ Could not parse distance from output: {result.stdout[:100]}...")
                    else:
                        print(f"   ❌ Distance command failed: {result.stderr[:100]}...")
                
                except subprocess.TimeoutExpired:
                    print(f"   ⏰ Distance analysis timeout")
                except Exception as e:
                    print(f"   ❌ Error running distance analysis: {e}")
    
    except Exception as e:
        print(f"❌ Error reading metadata: {e}")
        return
    
    # Analysis and correlation
    if not results:
        print("❌ No successful distance measurements")
        return
    
    print(f"\n📊 Analysis Results ({len(results)} successful measurements)")
    print("=" * 60)
    
    # Statistical summary
    errors = [r['relative_error'] for r in results]
    nadir_devs = [r['nadir_deviation'] for r in results]
    focal_lengths = [r['focal_length'] for r in results]
    distances = [r['true_distance'] for r in results]
    num_pixels = [r['num_pixels'] for r in results]
    
    print(f"📈 Error Statistics:")
    print(f"   Mean relative error: {np.mean(errors):.2f}%")
    print(f"   Std deviation: {np.std(errors):.2f}%")
    print(f"   Min error: {np.min(errors):.2f}%")
    print(f"   Max error: {np.max(errors):.2f}%")
    
    print(f"\n📐 Parameter Ranges:")
    print(f"   Nadir deviation: {np.min(nadir_devs):.1f}° to {np.max(nadir_devs):.1f}°")
    print(f"   Focal length: {np.min(focal_lengths)*1000:.1f}mm to {np.max(focal_lengths)*1000:.1f}mm")
    print(f"   True distance: {np.min(distances)/1000:.1f}km to {np.max(distances)/1000:.1f}km")
    print(f"   Num pixels: {np.min(num_pixels):,} to {np.max(num_pixels):,}")
    
    # Correlation analysis
    print(f"\n🔗 Correlation Analysis:")
    correlations = {
        'Nadir Deviation vs Error': np.corrcoef(nadir_devs, errors)[0,1],
        'Focal Length vs Error': np.corrcoef(focal_lengths, errors)[0,1],
        'Distance vs Error': np.corrcoef(distances, errors)[0,1],
        'Num Pixels vs Error': np.corrcoef(num_pixels, errors)[0,1]
    }
    
    for param, corr in correlations.items():
        strength = "Strong" if abs(corr) > 0.7 else "Moderate" if abs(corr) > 0.3 else "Weak"
        direction = "positive" if corr > 0 else "negative"
        print(f"   {param}: {corr:.3f} ({strength} {direction})")
    
    # Save detailed results
    results_file = output_dir / "distance_analysis_results.csv"
    try:
        import csv
        with open(results_file, 'w', newline='') as csvfile:
            if results:
                fieldnames = results[0].keys()
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                writer.writerows(results)
        print(f"\n💾 Detailed results saved to: {results_file}")
    except Exception as e:
        print(f"⚠️  Could not save CSV results: {e}")
    
    print(f"\n🎯 Best Performing Images:")
    sorted_results = sorted(results, key=lambda x: abs(x['relative_error']))
    for i, result in enumerate(sorted_results[:3]):
        print(f"   #{i+1}: {result['image_file']} - Error: {result['relative_error']:.2f}% (Nadir dev: {result['nadir_deviation']:.1f}°)")
    
    print(f"\n📉 Worst Performing Images:")
    for i, result in enumerate(sorted_results[-3:]):
        print(f"   #{i+1}: {result['image_file']} - Error: {result['relative_error']:.2f}% (Nadir dev: {result['nadir_deviation']:.1f}°)")


def create_output_directory(base_name: str = "simulation") -> Path:
    """Create a timestamped output directory for simulation results."""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    experiment_name = f"{base_name}_{timestamp}"
    
    # Create output directory in simulator/data/experiment_name/
    simulator_root = Path(__file__).parent.parent  # Go up from src/ to simulator/
    data_dir = simulator_root / "data"
    output_dir = data_dir / experiment_name
    
    # Create the directory structure with proper permissions
    output_dir.mkdir(parents=True, exist_ok=True, mode=0o755)
    
    print(f"📁 Created output directory: {output_dir}")
    return output_dir


def save_command_metadata(output_dir: Path, image_filename: str, command_list: list):
    """Save the generator command for an image to metadata file."""
    metadata_file = output_dir / "image_commands.txt"
    
    # Convert command list to string
    command_str = ' '.join(command_list)
    
    # Append to metadata file
    with open(metadata_file, 'a') as f:
        f.write(f"{image_filename} | {command_str}\n")


def run_quick_test():
    """Run a quick test with minimal parameters."""
    print("🧪 Running Quick Test Simulation")
    print("=" * 40)
    
    # Create output directory
    output_dir = create_output_directory("quick_test")
    
    # Simple test parameters (similar to test.py)
    distance = 10_000_000  # 10 Mm
    fov = np.radians(90)   # 90 degrees
    resolution = 1024      # 1024x1024 for speed
    
    # Calculate focal length
    focal_length = focal_length_from_fov_and_resolution(
        fov,  # half_fov
        resolution,
        DEFAULT_PIXEL_SIZE
    )
    
    print(f"📐 Test Parameters:")
    print(f"   Distance: {distance/1_000_000:.1f} Mm")
    print(f"   FOV: {np.degrees(fov):.1f}°")
    print(f"   Resolution: {resolution}x{resolution} px")
    print(f"   Focal length: {focal_length*1000:.2f} mm")
    
    # Generate just 2 orientations for quick test
    command_flags = generate_command_flags(
        distance=distance,
        fov=fov,
        num_cam_angles=3,
        num_spins=8,
        padding=0.1
    )
    
    print(f"\n🚀 Generating {len(command_flags)} test images...")
    
    for i, cmd_flags in enumerate(command_flags):
        output_file = output_dir / f"test_img_{i:02d}"
        
        generator_cmd = [
            "python", "-m", "tools.generator",
            *cmd_flags,
            "--filename", str(output_file),
            "--focal-length", f"{focal_length:.6f}",
            "--x-resolution", str(resolution),
            "--y-resolution", str(resolution),
            "--pixel-size", f"{DEFAULT_PIXEL_SIZE:.2e}",
            
        ]

        print(generator_cmd) # Debugging output
        
        print(f"   Generating image {i+1}/{len(command_flags)}...")
        print(f"   Command: {' '.join(generator_cmd[:8])}...")
        
        try:
            result = subprocess.run(
                generator_cmd,
                cwd=Path(__file__).parent.parent.parent.parent,  # Go to project root
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0:
                print(f"   ✅ Success: {output_file}.png")
            else:
                print(f"   ❌ Failed (code {result.returncode})")
                if result.stderr:
                    print(f"   Error: {result.stderr[:150]}...")
                if result.stdout:
                    print(f"   Output: {result.stdout[:150]}...")
        except subprocess.TimeoutExpired:
            print(f"   ⏰ Timeout: Image {i+1} took too long")
        except Exception as e:
            print(f"   ❌ Error: {str(e)[:100]}...")
    
    print(f"\n✅ Quick test complete! Check {output_dir}/")
    return output_dir


def run_default_simulation():
    """Run the default simulation using all combinations of parameters from constants.py."""
    print("🛰️  Starting Default Satellite Imaging Simulation (All Combinations)")
    print("=" * 60)
    
    # Get default simulation parameters from constants
    default_distances = SIMULATION_RANGES['distances'][DEFAULT_SIMULATION['distances']]
    default_fovs = np.radians(SIMULATION_RANGES['fovs'][DEFAULT_SIMULATION['fovs']])
    default_resolutions = SIMULATION_RANGES['resolutions'][DEFAULT_SIMULATION['resolutions']]
    
    print(f"📐 Default Simulation Parameters:")
    print(f"   Distance range: {DEFAULT_SIMULATION['distances']} -> {len(default_distances)} values")
    print(f"   FOV range: {DEFAULT_SIMULATION['fovs']} -> {len(default_fovs)} values")
    print(f"   Resolution range: {DEFAULT_SIMULATION['resolutions']} -> {len(default_resolutions)} values")
    
    # Calculate total combinations
    total_combinations = len(default_distances) * len(default_fovs) * len(default_resolutions)
    print(f"\n🔢 Total parameter combinations: {total_combinations}")
    
    # Create output directory
    output_dir = create_output_directory("default_full")
    
    # Create metadata file to track all commands
    metadata_file = output_dir / "image_commands.txt"
    
    # Generate all combinations of parameters
    param_combinations = list(itertools.product(default_distances, default_fovs, default_resolutions))
    
    print(f"\n🚀 Generating images for {len(param_combinations)} parameter combinations...")
    print(f"   Each combination will generate multiple camera angles/orientations")
    print(f"   Command metadata will be saved to: {metadata_file}")
    
    image_counter = 0
    
    # Write header to metadata file
    with open(metadata_file, 'w') as f:
        f.write("# Satellite Imaging Simulation - Command Metadata\n")
        f.write(f"# Generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"# Total parameter combinations: {len(param_combinations)}\n")
        f.write("# Format: IMAGE_FILE | FULL_COMMAND\n")
        f.write("# " + "="*80 + "\n\n")
    
    for combo_idx, (distance, fov, resolution) in enumerate(param_combinations):
        print(f"\n🎯 Combination {combo_idx + 1}/{len(param_combinations)}:")
        print(f"   Distance: {distance/1_000_000:.1f} Mm")
        print(f"   FOV: {np.degrees(fov):.1f}°")
        print(f"   Resolution: {resolution}x{resolution} px")
        
        # Calculate focal length for this combination
        focal_length = focal_length_from_fov_and_resolution(
            fov,
            resolution,
            DEFAULT_PIXEL_SIZE
        )
        
        print(f"   Focal length: {focal_length*1000:.2f} mm")
        
        # Generate command flags for this parameter combination
        command_flags = generate_command_flags(
            distance=distance,
            fov=fov,
            num_cam_angles=3,
            num_spins=8,
            padding=0.1
        )
        
        print(f"   Generating {len(command_flags)} images for this combination...")
        
        for angle_idx, cmd_flags in enumerate(command_flags):
            image_counter += 1
            output_file = output_dir / f"img_{image_counter:04d}_combo{combo_idx+1:02d}_angle{angle_idx+1:02d}"
            
            generator_cmd = [
                "python", "-m", "tools.generator",
                *cmd_flags,
                "--filename", str(output_file),
                "--focal-length", f"{focal_length:.6f}",
                "--x-resolution", str(resolution),
                "--y-resolution", str(resolution),
                "--pixel-size", f"{DEFAULT_PIXEL_SIZE:.2e}",
                "--grayscale"  # Add grayscale flag for smaller file sizes
            ]
            
            # Save command to metadata file
            with open(metadata_file, 'a') as f:
                image_name = f"img_{image_counter:04d}_combo{combo_idx+1:02d}_angle{angle_idx+1:02d}.png"
                f.write(f"{image_name} | {' '.join(generator_cmd)}\n")
            
            print(f"     Image {image_counter} (angle {angle_idx+1}/{len(command_flags)})...", end=" ")
        
            try:
                result = subprocess.run(
                    generator_cmd,
                    cwd=Path(__file__).parent.parent.parent.parent,
                    capture_output=True,
                    text=True,
                    timeout=60
                )
                
                if result.returncode == 0:
                    print("✅")
                else:
                    print("❌")
                    print(f"     Error: {result.stderr[:100]}...")
                    
            except Exception as e:
                print("❌")
                print(f"     Error: {str(e)[:100]}...")
    
    print(f"\n✅ Full simulation complete! Generated {image_counter} total images.")
    print(f"   Output directory: {output_dir}/")
    print(f"   Parameter combinations tested: {len(param_combinations)}")
    return output_dir


def main():
    """Main function with simulation options."""
    print("🛰️  Satellite Imaging Simulator")
    print("=" * 40)
    print("Choose simulation mode:")
    print("1. Quick test (2 images, fast)")
    print("2. Full parameter sweep (all combinations - many images!)")
    print("3. Post-analysis tools (lookup/reproduce specific images)")
    print("4. Exit")
    
    try:
        choice = input("\nEnter choice (1-4): ").strip()
        
        if choice == "1":
            run_quick_test()
        elif choice == "2":
            # Get default simulation parameters to show user what they're getting into
            default_distances = SIMULATION_RANGES['distances'][DEFAULT_SIMULATION['distances']]
            default_fovs = SIMULATION_RANGES['fovs'][DEFAULT_SIMULATION['fovs']]
            default_resolutions = SIMULATION_RANGES['resolutions'][DEFAULT_SIMULATION['resolutions']]
            
            total_combinations = len(default_distances) * len(default_fovs) * len(default_resolutions)
            images_per_combo = 4  # 2 cam_angles * 2 spins
            total_images = total_combinations * images_per_combo
            
            print(f"\n⚠️  WARNING: This will generate approximately {total_images} images!")
            print(f"   ({total_combinations} parameter combinations × {images_per_combo} angles each)")
            print(f"   This may take a very long time and use significant disk space.")
            
            confirm = input("Continue? (y/N): ").strip().lower()
            if confirm in ['y', 'yes']:
                run_default_simulation()
            else:
                print("Operation cancelled.")
        elif choice == "3":
            # Post-analysis tools
            print("\n🔍 Post-Analysis Tools")
            print("=" * 30)
            
            # Ask for output directory
            output_dir_str = input("Enter path to simulation output directory: ").strip()
            output_dir = Path(output_dir_str)
            
            if not output_dir.exists():
                print(f"❌ Directory not found: {output_dir}")
                return
            
            metadata_file = output_dir / "image_commands.txt"
            if not metadata_file.exists():
                print(f"❌ No metadata file found in {output_dir}")
                print("   This directory may not contain simulation results.")
                return
            
            print("Choose analysis option:")
            print("1. Look up command for specific image")
            print("2. Reproduce specific image")
            print("3. Analyze simulation results summary")
            
            analysis_choice = input("Enter choice (1-3): ").strip()
            
            if analysis_choice == "1":
                image_name = input("Enter image name (e.g., img_0001_combo01_angle01.png): ").strip()
                command = lookup_image_command(output_dir, image_name)
                if command:
                    print(f"\n✅ Command for {image_name}:")
                    print(f"   {command}")
                    
            elif analysis_choice == "2":
                image_name = input("Enter image name to reproduce: ").strip()
                new_path = input("Enter new output path (optional, press Enter to use original): ").strip()
                new_path = new_path if new_path else None
                reproduce_image(output_dir, image_name, new_path)
                
            elif analysis_choice == "3":
                analyze_simulation_results(output_dir)
                
            else:
                print("❌ Invalid choice.")
                
        elif choice == "4":
            print("👋 Goodbye!")
            return
        else:
            print("❌ Invalid choice. Running quick test by default.")
            run_quick_test()
            
    except KeyboardInterrupt:
        print("\n👋 Simulation interrupted by user.")
    except Exception as e:
        print(f"\n❌ Simulation failed: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()