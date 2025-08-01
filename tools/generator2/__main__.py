#!/usr/bin/env python3
"""
Generator2 Tool - Main Entry Point

Command-line interface for generating synthetic Earth images from space-based cameras.
This module provides argument parsing, validation, and orchestrates the complete
image generation pipeline using the modular components.

Usage:
    python -m tools.generator2 --position -10000000 0 0 --orientation 0 -90 0 [options]

For detailed help:
    python -m tools.generator2 --help
"""

import argparse
import logging
import sys
from pathlib import Path
import numpy as np

# Import our modular components
from .src.models import WGS84Spheroid, EARTH_EQUATORIAL_RADIUS
from .src.camera import Camera
from .src.rendering import EarthRenderer

# Configure logging
logging.basicConfig(level=logging.INFO, format="[%(levelname)s]: %(message)s")

# Default camera parameters
DEFAULT_FOCAL_LENGTH = 35.0  # mm
DEFAULT_PIXEL_SIZE = 0.01    # mm
DEFAULT_IMAGE_WIDTH = 1920   # pixels
DEFAULT_IMAGE_HEIGHT = 1080  # pixels


def create_argument_parser() -> argparse.ArgumentParser:
    """
    Create and configure the command-line argument parser.
    
    Returns:
        Configured ArgumentParser instance with all required and optional arguments
    """
    parser = argparse.ArgumentParser(
        description="Generate synthetic 2D images of Earth as seen from a camera",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic usage with required parameters
  python -m tools.generator2 --position -10000000 0 0 --orientation 0 -90 0
  
  # With custom camera settings and wide field of view
  python -m tools.generator2 --position -8000000 0 0 --orientation 0 -90 0 \\
      --focal-length 10.0 --pixel-size 0.05 --image-size 1280 720
  
  # Save to specific file with verbose output
  python -m tools.generator2 --position -7000000 0 0 --orientation 0 -90 0 \\
      --focal-length 5.0 --pixel-size 0.1 --output earth_view.png --verbose

Camera Parameter Guidelines:
  - Wider field of view (shorter focal length, larger pixels) captures more horizon
  - Narrow field of view (longer focal length, smaller pixels) zooms in on Earth
  - For horizon capture from 7-15Mm distance, use focal length ≤ 10mm
  - Position must be outside Earth's radius (≥ 6.38Mm from center)
  
Field of View Examples:
  - f=35mm, px=0.01mm → ~31° diagonal (narrow, good for distant Earth)
  - f=10mm, px=0.05mm → ~87° diagonal (medium, balanced view)  
  - f=5mm, px=0.1mm → ~120° diagonal (wide, captures horizon easily)
        """
    )
    
    # Required arguments
    parser.add_argument('--position', nargs=3, type=float, required=True,
                       metavar=('X', 'Y', 'Z'),
                       help='Camera position [x, y, z] in meters (must be outside Earth)')
    
    parser.add_argument('--orientation', nargs=3, type=float, required=True,
                       metavar=('YAW', 'PITCH', 'ROLL'),
                       help='Camera orientation [yaw, pitch, roll] in degrees (Euler angles)')
    
    # Optional camera parameters
    parser.add_argument('--focal-length', type=float, default=DEFAULT_FOCAL_LENGTH,
                       metavar='MM',
                       help=f'Focal length in millimeters (default: {DEFAULT_FOCAL_LENGTH}). '
                            'Shorter = wider field of view')
    
    parser.add_argument('--pixel-size', type=float, default=DEFAULT_PIXEL_SIZE,
                       metavar='MM', 
                       help=f'Pixel size in millimeters (default: {DEFAULT_PIXEL_SIZE}). '
                            'Larger = wider field of view')
    
    parser.add_argument('--image-size', nargs=2, type=int, 
                       default=[DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT],
                       metavar=('WIDTH', 'HEIGHT'),
                       help=f'Image size [width, height] in pixels '
                            f'(default: {DEFAULT_IMAGE_WIDTH} {DEFAULT_IMAGE_HEIGHT})')
    
    # Output options
    parser.add_argument('--output', '-o', type=str,
                       metavar='PATH',
                       help='Output image file path (default: auto-generated based on parameters)')
    
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose logging with debug information')
    
    return parser


def validate_arguments(args) -> None:
    """
    Validate command-line arguments for physical and geometric consistency.
    
    Args:
        args: Parsed command-line arguments
        
    Raises:
        ValueError: If arguments are invalid or inconsistent
        SystemExit: If validation fails (calls sys.exit)
    """
    # Validate camera position (must be outside Earth)
    position = np.array(args.position)
    distance_from_earth = np.linalg.norm(position)
    
    if distance_from_earth <= EARTH_EQUATORIAL_RADIUS:
        logging.error(f"Camera position is inside Earth!")
        logging.error(f"  Distance from center: {distance_from_earth:.0f} m")
        logging.error(f"  Earth radius: {EARTH_EQUATORIAL_RADIUS:.0f} m")
        logging.error(f"  Required minimum distance: {EARTH_EQUATORIAL_RADIUS + 1000:.0f} m")
        sys.exit(1)
    
    # Validate camera parameters
    if args.focal_length <= 0:
        logging.error(f"Focal length must be positive, got {args.focal_length}mm")
        sys.exit(1)
    
    if args.pixel_size <= 0:
        logging.error(f"Pixel size must be positive, got {args.pixel_size}mm")
        sys.exit(1)
    
    if args.image_size[0] <= 0 or args.image_size[1] <= 0:
        logging.error(f"Image dimensions must be positive, got {args.image_size[0]}×{args.image_size[1]}")
        sys.exit(1)
    
    # Log camera configuration info
    logging.info(f"Camera position: ({position[0]:.0f}, {position[1]:.0f}, {position[2]:.0f}) m")
    logging.info(f"Distance from Earth: {distance_from_earth:.0f} m ({distance_from_earth/1000:.0f} km)")


def generate_output_filename(position: list, orientation: list) -> str:
    """
    Generate a descriptive filename based on camera parameters.
    
    Args:
        position: Camera position [x, y, z]
        orientation: Camera orientation [yaw, pitch, roll]
        
    Returns:
        Auto-generated filename string
    """
    pos_str = f"({position[0]:.2f}, {position[1]:.2f}, {position[2]:.2f})"
    ori_str = f"({orientation[0]:.2f}, {orientation[1]:.2f}, {orientation[2]:.2f})"
    return f"{pos_str},{ori_str}.png"


def display_final_statistics(stats: dict) -> None:
    """
    Display comprehensive statistics about the generated image.
    
    Args:
        stats: Statistics dictionary from the renderer
    """
    print(f"\n📊 Earth Image Generation Statistics:")
    print(f"   🌍 Earth fills {stats['earth_fill_percentage']:.2f}% of the image")
    print(f"   🔭 Earth's angular size: {stats['angular_radius_degrees']:.3f}°")
    print(f"   🌅 Horizon captured: {'✓' if stats['horizon_captured'] else '✗'}")
    
    if stats['horizon_captured']:
        print(f"   🌐 Horizon coverage: {stats['horizon_capture_percentage']:.1f}%")
    
    print(f"   📐 Image resolution: {stats['image_width']}×{stats['image_height']} pixels")
    print(f"   🎯 Earth area: {stats['earth_area_pixels']:.0f} pixels")
    
    # Provide guidance if horizon not captured
    if not stats['horizon_captured']:
        print(f"\n⚠️  Horizon Visibility Warning:")
        print(f"   Earth's horizon is not visible in this camera view.")
        print(f"   Consider adjusting camera parameters:")
        print(f"   • Decrease focal length (e.g., 5-10mm) for wider field of view")
        print(f"   • Increase pixel size (e.g., 0.05-0.1mm) for wider field of view")
        print(f"   • Move camera further from Earth for smaller angular size")


def main():
    """
    Main entry point for the generator2 tool.
    
    Orchestrates the complete pipeline:
    1. Parse and validate command-line arguments
    2. Initialize Earth and camera models
    3. Generate and save Earth image
    4. Display comprehensive statistics
    """
    # Parse command-line arguments
    parser = create_argument_parser()
    args = parser.parse_args()
    
    # Configure logging level
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
        logging.debug("Verbose logging enabled")
    
    # Validate arguments
    validate_arguments(args)
    
    # Generate output filename if not provided
    if args.output is None:
        args.output = generate_output_filename(args.position, args.orientation)
        logging.info(f"Auto-generated output filename: {args.output}")
    
    # Ensure output directory exists
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    
    try:
        # Initialize models
        logging.info("Initializing camera and Earth models...")
        
        camera = Camera(
            position=args.position,
            orientation=args.orientation,
            focal_length=args.focal_length,
            pixel_size=args.pixel_size,
            image_width=args.image_size[0],
            image_height=args.image_size[1]
        )
        
        spheroid = WGS84Spheroid()
        
        # Log camera info
        if args.verbose:
            h_fov, v_fov = camera.get_field_of_view()
            d_fov = camera.get_diagonal_fov()
            logging.debug(f"Camera field of view: {h_fov:.1f}°×{v_fov:.1f}° (diagonal: {d_fov:.1f}°)")
        
        # Create renderer and generate image
        logging.info("Starting Earth image generation...")
        renderer = EarthRenderer(camera, spheroid)
        stats = renderer.render_image(str(output_path))
        
        # Display results
        display_final_statistics(stats)
        
        logging.info("Earth image generation completed successfully!")
        
    except ValueError as e:
        logging.error(f"Configuration error: {e}")
        sys.exit(1)
    except Exception as e:
        logging.error(f"Failed to generate Earth image: {e}")
        if args.verbose:
            import traceback
            logging.debug(f"Detailed error trace:\n{traceback.format_exc()}")
        sys.exit(1)


if __name__ == '__main__':
    main()
