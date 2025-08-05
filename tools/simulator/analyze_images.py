#!/usr/bin/env python3
"""
Quick analysis script for simulation results.

Usage examples:
    # Look up command for specific image
    python analyze_images.py lookup /path/to/output/dir img_0001_combo01_angle01.png
    
    # Reproduce specific image
    python analyze_images.py reproduce /path/to/output/dir img_0001_combo01_angle01.png new_output_name
    
    # Analyze all results
    python analyze_images.py summary /path/to/output/dir
"""

import sys
from pathlib import Path

# Add the src directory to path to import our modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

from __main__ import lookup_image_command, reproduce_image, analyze_simulation_results


def main():
    if len(sys.argv) < 3:
        print("Usage:")
        print("  python analyze_images.py lookup <output_dir> <image_name>")
        print("  python analyze_images.py reproduce <output_dir> <image_name> [new_output]")
        print("  python analyze_images.py summary <output_dir>")
        return
    
    command = sys.argv[1]
    output_dir = Path(sys.argv[2])
    
    if command == "lookup":
        if len(sys.argv) < 4:
            print("❌ Missing image name")
            return
        image_name = sys.argv[3]
        cmd = lookup_image_command(output_dir, image_name)
        if cmd:
            print(f"✅ Command for {image_name}:")
            print(cmd)
    
    elif command == "reproduce":
        if len(sys.argv) < 4:
            print("❌ Missing image name")
            return
        image_name = sys.argv[3]
        new_output = sys.argv[4] if len(sys.argv) > 4 else None
        reproduce_image(output_dir, image_name, new_output)
    
    elif command == "summary":
        analyze_simulation_results(output_dir)
    
    else:
        print(f"❌ Unknown command: {command}")


if __name__ == "__main__":
    main()
