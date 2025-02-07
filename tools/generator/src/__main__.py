import argparse
import logging

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")

from common.constants import (
    DEFAULT_FOCAL_LEN,
    DEFAULT_RESOLUTION,
    DEFAULT_PIXEL_SIZE,
    DEFAULT_LEO_THRESHOLD,
    EARTH_RADIUS,
)
from spatial.coordinate import Attitude, Vector
from spatial.camera import Camera
from curve.spherical import SphericalCurveProvider
from image.printer import Printer


if __name__ == "__main__":
    """Runs the generator script to get an image of earth.
    We use the celestial coordinate system and assume Earth is a sphere

    pythom -m generator <x_position> <y_position> <z_position> <declination> <right_ascension> <roll> --focal_length [focal_length] --x_resolution [x_resolution] --y_resolution [y_resolution]

    Args:
        x_position (float): The x position of the satellite
        y_posiiton (float): The y position of the satellite
        z_position (float): The z position of the satellite
        declination (float): The declination of the camera
        right_ascension (float): The right ascention of the camera
        roll (float): The roll of the camera
        focal_length (float, Optional): The focal length of the camera. Defaults to __
        x_resolution (float, Optional): The x resolution of the camera. Defaults to 1024
        y_resolution (float, Optional): The y resolution of the camera. Defaults to 1024
    """
    # Step 1: Parse out Arguments
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
    args = parser.parse_args()

    position, orientation = Vector(*args.position), Attitude(
        *args.orientation, radians=False
    )

    # Step 1: Figure out if the parameters are okay
    if position.norm < EARTH_RADIUS:
        logging.error(f"The position {position} is invalid (not above earth)")
        exit(1)
    if position.norm < args.leo_threshold:
        logging.warning(f"The position {position} is below LEO")

    # Step 2: Figure out camera coordinate system with respect to celestial coordinate system
    camera = Camera(
        position,
        orientation,
        args.focal_length,
        args.pixel_size,
        args.x_resolution,
        args.y_resolution,
    )

    # Step 3: Get curve points from earth wrt celestial coordinate system
    spherical_curve_provider = SphericalCurveProvider(position)
    center, points = spherical_curve_provider.generate_points()

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
            f"The camera parameters may not be sufficient to capture Earth's radius (the sensor width to focal length ratio is {camera.pixel_length * max(camera.x_resolution, camera.y_resolution) / camera.focal_length:.3f}, but the edge radius to center distance is {spherical_curve_provider.radius.norm / (position - spherical_curve_provider.center).norm:.3f})"
        )

    # Step 4: Transform points into camera coordinate system
    camera_points = camera.to_camera_space(points)
    camera_center = camera.to_camera_space({center})

    # for pt in camera_points:
    #     print(pt)

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
            # print(pt)
            count += 1
    if count == 0:
        logging.error(
            "The produced image does not capture Earth's edge. Skipping image output."
        )
        exit(1)
    if len(image_points) != len(camera_points):
        logging.warning("The produced image cuts off negative points")
    print(f"{count / len(image_points) * 100}% of the horizon is in the camera")

    # Step 6: Draw and Save Image
    printer = Printer(camera)
    printer.generate_image(image_points)
    filename = args.filename if args.filename else f"{position},{orientation}"
    printer.save_image(filename)
