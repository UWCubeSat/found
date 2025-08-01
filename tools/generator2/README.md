# Generator2 Tool

A sophisticated command-line utility for generating synthetic 2D images of Earth as seen from space-based cameras. This tool models Earth as a WGS84 oblate spheroid and simulates finite projective pinhole cameras with configurable parameters.

## Overview

The Generator2 tool is designed for satellite mission planning, computer vision testing, and educational purposes. It generates realistic Earth images from any orbital position, accounting for proper geometric perspective, horizon visibility, and Earth coverage statistics.

## Features

- **WGS84 Earth Model**: Accurate oblate spheroid representation with proper geometric calculations
- **Finite Projective Camera**: Configurable pinhole camera with focal length, pixel size, and orientation controls
- **Horizon Detection**: Automatic detection and rendering of Earth's horizon circle
- **Pixel-Perfect Analysis**: Accurate Earth coverage statistics using image analysis
- **Clean Image Output**: Pure Earth images without UI elements, titles, or scales
- **Comprehensive Statistics**: Detailed reporting of Earth coverage, horizon visibility, and angular measurements
- **Modular Architecture**: Well-organized codebase with separate modules for models, camera, rendering, and analysis

## Architecture

The tool is organized into modular components:

```
tools/generator2/
├── src/
│   ├── __init__.py          # Package initialization and exports
│   ├── __main__.py          # Main CLI entry point
│   ├── models/              # Earth modeling components
│   │   ├── __init__.py      # Earth model exports
│   │   └── earth_model.py   # WGS84Spheroid implementation
│   ├── camera/              # Camera modeling components
│   │   ├── __init__.py      # Camera exports
│   │   └── pinhole_camera.py # Camera class implementation
│   ├── rendering/           # Image generation components
│   │   ├── __init__.py      # Rendering exports
│   │   └── earth_renderer.py # EarthRenderer implementation
│   └── analysis/            # Image analysis components
│       ├── __init__.py      # Analysis exports
│       └── image_analyzer.py # ImageAnalyzer implementation
├── tests/                   # Unit and integration tests
└── README.md               # This documentation
```

## Earth Model

The WGS84Spheroid class (`models/earth_model.py`) provides accurate Earth geometry calculations:
- Equatorial radius: 6,378,137.0 meters
- Polar radius: 6,356,752.3 meters

## Camera Model

The finite projective pinhole camera supports the following configurable parameters:
- **Position**: 3D coordinates (x, y, z) in meters from Earth's center
- **Orientation**: Euler angles (yaw, pitch, roll) in degrees
- **Focal Length**: Camera focal length in millimeters (default: 35.0)
- **Pixel Size**: Physical pixel size in millimeters (default: 0.01)
- **Image Resolution**: Width and height in pixels (default: 1920x1080)

## Usage

### Basic Command

```bash
python -m tools.generator2 --position -10000000 0 0 --orientation -70.1 -70.1 0
```

### Full Command with All Options

```bash
python -m tools.generator2 \
    --position -10000000 0 0 \
    --orientation -70.1 -70.1 0 \
    --focal-length 50.0 \
    --pixel-size 0.005 \
    --image-size 2560 1440 \
    --output custom_earth_view.png \
    --verbose
```

### Command Line Arguments

| Argument | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `--position` | 3 floats | Yes | - | Camera position [x, y, z] in meters from Earth center |
| `--orientation` | 3 floats | Yes | - | Camera orientation [yaw, pitch, roll] in degrees |
| `--focal-length` | float | No | 35.0 | Focal length in millimeters |
| `--pixel-size` | float | No | 0.01 | Pixel size in millimeters |
| `--image-size` | 2 ints | No | 1920 1080 | Image dimensions [width, height] in pixels |
| `--output` | string | No | auto-generated | Output image file path |
| `--verbose` | flag | No | false | Enable verbose logging |

## Examples

### Low Earth Orbit View
```bash
python -m tools.generator2 --position -7000000 0 0 --orientation 0 -90 0
```

### Geostationary Orbit View
```bash
python -m tools.generator2 --position -42164000 0 0 --orientation 0 -90 0
```

### Angled View with Custom Camera Settings
```bash
python -m tools.generator2 \
    --position -10000000 5000000 2000000 \
    --orientation -30 -60 15 \
    --focal-length 85.0 \
    --pixel-size 0.004 \
    --image-size 3840 2160
```

## Output

The tool generates a PNG image showing:
- **Earth Surface**: Blue points representing the visible Earth surface
- **Horizon Line**: Red line showing the Earth's horizon as seen from the camera
- **Metadata**: Image title includes camera position and orientation information

Output files are automatically named based on position and orientation if no custom filename is provided:
```
(-10000000.00, 0.00, 0.00),(-70.10, -70.10, 0.00).png
```

## Environment Setup

The tool requires the `generator2_env` conda environment. If using the dev container, environments are pre-configured. Otherwise, run:

```bash
# From the found/ directory
./tools/setup-tools-envs.sh
```

Then activate the environment:
```bash
micromamba activate generator2_env
```

## Dependencies

- Python ≥ 3.9
- NumPy (numerical computations)
- Matplotlib (image generation and plotting)
- OpenCV (image processing support)
- pytest (testing framework)

## Coordinate Systems

- **World Coordinates**: Earth-centered coordinate system with origin at Earth's center
- **Camera Coordinates**: Right-handed coordinate system with Z-axis pointing toward the camera's view direction
- **Image Coordinates**: 2D pixel coordinates with origin at top-left corner

## Limitations

- Camera position must be outside Earth's surface
- Very distant positions may result in Earth appearing as a single pixel
- Atmospheric effects are not modeled
- Earth is rendered as a solid body without surface features or textures

## Testing

Run the test suite with:
```bash
python -m pytest tools/generator2/test/
```

## Implementation Notes

This tool is completely independent from `tools/generator/` and shares no code. It implements its own:
- WGS84 oblate spheroid mathematics
- Camera projection algorithms  
- Image rendering pipeline
- Command-line interface

The implementation focuses on accuracy of the Earth model and camera projection while maintaining simplicity and performance.
