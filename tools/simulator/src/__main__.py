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
    EARTH_MEAN_RADIUS
)
from common.utility import focal_length_from_fov_and_resolution


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
        num_cam_angles=2,
        num_spins=1,
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
            "--pixel-size", f"{DEFAULT_PIXEL_SIZE:.2e}"
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
    """Run the default simulation using parameters from constants.py."""
    print("🛰️  Starting Default Satellite Imaging Simulation")
    print("=" * 60)
    
    # Get default simulation parameters from constants
    default_distances = SIMULATION_RANGES['distances'][DEFAULT_SIMULATION['distances']]
    default_fovs = np.radians(SIMULATION_RANGES['fovs'][DEFAULT_SIMULATION['fovs']])
    default_resolutions = SIMULATION_RANGES['resolutions'][DEFAULT_SIMULATION['resolutions']]
    
    print(f"📐 Default Simulation Parameters:")
    print(f"   Distance range: {DEFAULT_SIMULATION['distances']} -> {len(default_distances)} values")
    print(f"   FOV range: {DEFAULT_SIMULATION['fovs']} -> {len(default_fovs)} values")
    print(f"   Resolution range: {DEFAULT_SIMULATION['resolutions']} -> {len(default_resolutions)} values")
    
    # Create output directory
    output_dir = create_output_directory("default")
    
    # Use middle values for demonstration
    sample_distance = default_distances[len(default_distances)//2]
    sample_fov = default_fovs[len(default_fovs)//2]
    sample_resolution = default_resolutions[len(default_resolutions)//2]
    
    print(f"\n🎯 Selected Parameters:")
    print(f"   Distance: {sample_distance/1_000_000:.1f} Mm")
    print(f"   FOV: {np.degrees(sample_fov):.1f}°")
    print(f"   Resolution: {sample_resolution}x{sample_resolution} px")
    
    # Calculate focal length
    focal_length = focal_length_from_fov_and_resolution(
        sample_fov / 2,
        sample_resolution,
        DEFAULT_PIXEL_SIZE
    )
    
    print(f"   Focal length: {focal_length*1000:.2f} mm")
    
    # Generate command flags
    command_flags = generate_command_flags(
        distance=sample_distance,
        fov=sample_fov,
        num_cam_angles=4,
        num_spins=2,
        padding=0.1
    )
    
    print(f"\n🚀 Generating {len(command_flags)} images...")
    
    for i, cmd_flags in enumerate(command_flags):
        output_file = output_dir / f"simulation_img_{i:03d}"
        
        generator_cmd = [
            "python", "-m", "tools.generator",
            *cmd_flags,
            "--filename", str(output_file),
            "--focal-length", f"{focal_length:.6f}",
            "--x-resolution", str(sample_resolution),
            "--y-resolution", str(sample_resolution),
            "--pixel-size", f"{DEFAULT_PIXEL_SIZE:.2e}"
        ]
        
        print(f"   Processing image {i+1}/{len(command_flags)}...")
        
        try:
            result = subprocess.run(
                generator_cmd,
                cwd=Path(__file__).parent.parent.parent.parent,
                capture_output=True,
                text=True,
                timeout=60
            )
            
            if result.returncode == 0:
                print(f"   ✅ Generated: {output_file}.png")
            else:
                print(f"   ❌ Failed: {result.stderr[:100]}...")
                
        except Exception as e:
            print(f"   ❌ Error: {str(e)[:100]}...")
    
    print(f"\n✅ Simulation complete! Check {output_dir}/")
    return output_dir


def main():
    """Main function with simulation options."""
    print("🛰️  Satellite Imaging Simulator")
    print("=" * 40)
    print("Choose simulation mode:")
    print("1. Quick test (2 images, fast)")
    print("2. Default simulation (8 images, full parameters)")
    print("3. Exit")
    
    try:
        choice = input("\nEnter choice (1-3): ").strip()
        
        if choice == "1":
            run_quick_test()
        elif choice == "2":
            run_default_simulation()
        elif choice == "3":
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