import argparse
import logging
from pathlib import Path
from typing import List, Tuple

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")

from common.constants import (
    DEFAULT_FOCAL_LEN,
    DEFAULT_RESOLUTION,
    DEFAULT_PIXEL_SIZE,
    DEFAULT_LEO_THRESHOLD,
    EARTH_RADIUS,
    NUM_EARTH_POINTS,
)
from spatial.coordinate import Attitude, Vector
from spatial.camera import Camera
from curve.spherical import SphericalCurveProvider
from image.printer import Printer


def parse_args() -> argparse.Namespace:
    """Parses out the arguments for this program

    Args:
        position (List[float]): The position of the satellite (meters, [x, y, z]).
        orientation (List[float]): The attitude of the satellite (degrees, [ra, de, roll]).
        leo-threshold (float, Optional): The lowest expected altitude from the surface of Earth (meters).
        focal-length (float, Optional): The focal length of the camera (meters). Defaults to 8.5e-3 meters.
        pixel-length (float, Optional): The physical length of a pixel (meters). Defaults to 20e-6 meters.
        x-resolution (float, Optional): The x resolution of the camera (pixels). Defaults to 6000 pixels.
        y-resolution (float, Optional): The y resolution of the camera (pixels). Defaults to 6000 pixels.
        filename (str, Optional): The filename (including path) of the output image. Defaults to a name that describes position and orientation, outputted directly into the repository (found) folder.

    Returns:
        argparse.Namespace: The arguments for this program
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--position",
        nargs=3,
        type=float,
        help="The position of the satellite (x, y, z)",
        required=True,
    )
    parser.add_argument(
        "--orientation",
        nargs=3,
        type=float,
        help="The orientation of the satellite (ra, de, roll)",
        required=True,
    )
    parser.add_argument(
        "--leo-threshold",
        type=float,
        default=DEFAULT_LEO_THRESHOLD,
        help="The lowest orbit altitude above earth's surface (in meters)",
    )
    parser.add_argument(
        "--focal-length",
        type=float,
        default=DEFAULT_FOCAL_LEN,
        help="The focal length of the camera (in meters)",
    )
    parser.add_argument(
        "--pixel-size",
        type=float,
        default=DEFAULT_PIXEL_SIZE,
        help="The pixel size (in m^2)",
    )
    parser.add_argument(
        "--x-resolution",
        type=int,
        default=DEFAULT_RESOLUTION,
        help="The x resolution of the camera (in pixels)",
    )
    parser.add_argument(
        "--y-resolution",
        type=int,
        default=DEFAULT_RESOLUTION,
        help="The y resolution of the camera (in pixels)",
    )
    parser.add_argument("--filename", default=None, help="The output file name")
    parser.add_argument(
        "--grayscale", 
        action="store_true", 
        help="Generate grayscale image instead of color"
    )
    args = parser.parse_args()

    position, orientation = Vector(*args.position), Attitude(
        *args.orientation, radians=False
    )

    return (
        position,
        orientation,
        args.leo_threshold,
        args.focal_length,
        args.pixel_size,
        args.x_resolution,
        args.y_resolution,
        (
            args.filename
            if args.filename
            else f"{Path(__file__).resolve().parents[3]}/{position},{orientation}"
        ),
        args.grayscale,
    )


def generate_points(
    position: Vector,
    orientation: Attitude,
    leo_threshold: float,
    focal_length: float,
    pixel_size: float,
    x_resolution: int,
    y_resolution: int,
    num_points: int = NUM_EARTH_POINTS,
) -> Tuple[Camera, List[Vector]]:
    """Generates the edge points of earth as seen in the camera given by all parameters

    Args:
        position (Vector): The position of the camera
        orientation (Attitude): The orientation of the camera
        leo_threshold (float): The low threshold for orbit altitude
        focal_length (float): The camera focal length
        pixel_size (float): The physical length of a pixel
        x_resolution (int): The number of pixels in the image along the x direction
        y_resolution (int): The number of pixels in the image along the y direction

    Returns:
        Tuple[Camera, List[Vector]]: The camera and the edge points generated on its image

    Note:
        The returned edge points are given with respect to the camera 2d coordinate system,
        not the graphics coordinate system (the origin is in the center of the image, pointing
        up and to the left)
    """
    # Step 1: Figure out if the parameters are okay
    if position.norm < EARTH_RADIUS:
        logging.error(f"The position {position} is invalid (not above earth)")
        exit(1)
    if position.norm < leo_threshold:
        logging.warning(f"The position {position} is below LEO")

    # Step 2: Figure out camera coordinate system with respect to celestial coordinate system
    camera = Camera(
        position,
        orientation,
        focal_length,
        pixel_size,
        x_resolution,
        y_resolution,
    )

    # Step 3: Get curve points from earth wrt celestial coordinate system
    spherical_curve_provider = SphericalCurveProvider(position)
    center, points = spherical_curve_provider.generate_points(num_points=num_points)

    # Step 3E: Calculate if its possible that a head on image of Earth may not
    # be feasible
    if (
        camera.pixel_length
        * max(camera.x_resolution, camera.y_resolution)
        / camera.focal_length
        < spherical_curve_provider.radius.norm
        / (position - spherical_curve_provider.center).norm
    ):
        logging.warning(
            f"The camera may not capture Earth's edge ([sensor width/focal length={camera.pixel_length * max(camera.x_resolution, camera.y_resolution) / camera.focal_length:.3f}] < [edge radius/center distance={spherical_curve_provider.radius.norm / (position - spherical_curve_provider.center).norm:.3f}])"
        )

    # Step 4: Transform points into camera coordinate system
    camera_points = camera.to_camera_space(points)
    camera_center = camera.to_camera_space({center})

    # Step 5: Project Points into Camera
    image_center, *image_points = camera.spatial_to_camera(
        camera_center, *camera_points
    )

    if image_center == None:
        logging.error(
            "The produced image does not capture Earth's edge. Skipping image output."
        )
        exit(1)
    count = 0
    for pt in image_points:
        if camera.in_camera(pt):
            count += 1
    if count == 0:
        logging.error(
            "The produced image does not capture Earth's edge. Skipping image output."
        )
        exit(1)
    if len(image_points) != len(camera_points):
        logging.warning(
            f"{(len(camera_points)-len(image_points)) / len(camera_points) * 100:.2f}% of the horizon is behind the camera"
        )
    logging.info(
        f"{count / len(camera_points) * 100:.2f}% of the horizon is in the camera"
    )

    return (
        camera,
        image_points,
    )


def print_image(
    camera: Camera,
    image_points: List[Vector],
    filename: str,
    grayscale: bool = False,
):
    """Generates an image

    Args:
        camera (Camera): The camera that took the image
        image_points (List[Vector]): The edge points that go onto the image
        filename (str): The file to save the image to
        grayscale (bool): Whether to generate a grayscale image

    Precondition:
        image_points is given with respect to the camera 2d axes
    """
    # Step 6: Draw and Save Image
    printer = Printer(camera, grayscale)
    printer.generate_image(image_points)
    printer.save_image(filename)


if __name__ == "__main__":
    """Runs the generator script to get an image of earth.
    We use the celestial coordinate system and assume Earth is a sphere

    pythom -m generator <x_position> <y_position> <z_position> <declination> <right_ascension> <roll> --focal_length [focal_length] --x_resolution [x_resolution] --y_resolution [y_resolution]

    Args:
        position (List[float]): The position of the satellite (meters, [x, y, z]).
        orientation (List[float]): The attitude of the satellite (degrees, [ra, de, roll]).
        leo-threshold (float, Optional): The lowest expected altitude from the surface of Earth (meters).
        focal-length (float, Optional): The focal length of the camera (meters). Defaults to 8.5e-3 meters.
        pixel-length (float, Optional): The physical length of a pixel (meters). Defaults to 20e-6 meters.
        x-resolution (float, Optional): The x resolution of the camera (pixels). Defaults to 6000 pixels.
        y-resolution (float, Optional): The y resolution of the camera (pixels). Defaults to 6000 pixels.
        filename (str, Optional): The filename (including path) of the output image. Defaults to a name that describes position and orientation, outputted directly into the repository (found) folder.
    """
    (
        position,
        orientation,
        leo_threshold,
        focal_length,
        pixel_size,
        x_resolution,
        y_resolution,
        filename,
        grayscale,
    ) = parse_args()

    camera, image_points = generate_points(
        position,
        orientation,
        leo_threshold,
        focal_length,
        pixel_size,
        x_resolution,
        y_resolution,
    )

    print_image(camera, image_points, filename, grayscale)
