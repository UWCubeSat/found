import numpy as np
import subprocess
import math

# Camera parameters     # Focal length in meters
DEFAULT_RESOLUTION = 6000      # Resolution in pixels
DEFAULT_PIXEL_SIZE = 20e-6     # Pixel size in meters
RADIUS = 6378137  # Earth radius in meters


half_fov = np.pi / 4
distance = 10000000  # Camera distance from sphere center

focal_length = (DEFAULT_RESOLUTION * DEFAULT_PIXEL_SIZE)/2 / np.tan(half_fov)
inscribed_angle = np.arcsin(RADIUS /distance)

padding = .1

lower = inscribed_angle - half_fov * (1 - padding)
upper = inscribed_angle + half_fov * (1 - padding)

absolute_lower = -inscribed_angle + half_fov * (1 + padding)
lower = max(lower, absolute_lower)

cam_angles = np.linspace(lower, upper, num=4)
print(f"camera angles (radians): {cam_angles}")
cam_angles = np.degrees(cam_angles)
print(f"camera angles (degrees): {cam_angles}")

spin = np.pi 
y_anles = np.sin(spin) * cam_angles
x_angles = np.cos(spin) * cam_angles

for i, p in enumerate(cam_angles):

    # Construct position and orientation
    position = f"{-distance} 0 0"
    orientation = f"{y_anles[i]:.2f} {x_angles[i]:.2f} 0"

    # Build and call the command
    cmd = [
        "python", "-m", "tools.generator",
        "--position", *position.split(),
        "--orientation", *orientation.split(),
        "--filename", f"tools/simulator/output_n{i}",
        "--focal-length", f"{focal_length:.2f}"
    ]
    print(f"Running: {' '.join(cmd)}")
    subprocess.run(cmd)

    # cmdDistance = [
    #     "./build/bin/found", "distance",
    #     "--image", f"tools/simulator/output_{i}.png",
    #     "--reference-as-orientation",
    #     "--camera-focal-length", str(DEFAULT_FOCAL_LEN),
    # "--camera-pixel-size", str(DEFAULT_PIXEL_SIZE),
    # "--reference-orientation", *orientation.split()
    # # ,"--distance-algo", "SDDA"
    # ]
    # #print(f"Running: {' '.join(cmdDistance)}")
    # #subprocess.run(cmdDistance)
