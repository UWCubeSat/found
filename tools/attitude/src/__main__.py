import argparse
import logging
from typing import Tuple, List, Optional

from common.constants import (
    CALIBRATION_BANNER,
    DISTANCE_BANNER,
    CALIBRATION_HEADER,
    LOCAL_HEADER,
    REFERENCE_HEADER
)

from transform.transform import Attitude, DCM

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")

def parse_args() -> Tuple[DCM, DCM, int]:
    """Parse Arguments for the tool

    Args:
        local_attitude (Optional[DCM]): The local attitude to use in the first test pair
        calibration_attitude (DCM): The calibration rotation (that rotates from the reference to local orientation)
        num_attitude_pairs (int): The number of test attitude pairs to generate

    Returns:
        Tuple[Attitude, DCM, int]: The arguments
    
    Note:
        We convert the local_attitude into a DCM for convenience
    """
    
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--local-attitude",
        nargs=3,
        type=float,
        help="The orientation of FOUND (ra, de, roll) in degrees. Must be supplied if --use-local is used",
        default=None
    )
    parser.add_argument(
        "--calibration-attitude",
        nargs=3,
        type=float,
        help="The euler angles for the calibration rotation, if you want to reuse a particular calibration",
        default=None
    )
    parser.add_argument(
        "--num-attitude-pairs",
        type=int,
        help="Number of test attitude pairs to generate. Must be at least 1",
        default=1
    )
    args = parser.parse_args()
    
    # Step 1: Produce Calibration Rotation
    if args.calibration_attitude is None:
        calibration_attitude = DCM()
    else:
        calibration_attitude = Attitude(*args.calibration_attitude).to_dcm()

    return DCM() if args.local_attitude is None else Attitude(*args.local_attitude).to_dcm(), calibration_attitude, args.num_attitude_pairs

def validate_arguments(local_attitude: Optional[DCM],
                       calibration_attitude: DCM,
                       num_attitude_pairs: int):
    """Validates the arguments

    Args:
        local_attitude (Optional[DCM]): The local attitude to use in the first test pair
        calibration_attitude (DCM): The calibration rotation
        num_attitude_pairs (int): The number of test attitude pairs to generate
    """
    if num_attitude_pairs < 1:
        raise RuntimeError("Must generate at least 1 test attitude pair")

def produce_attitudes(local_attitude: Optional[DCM],
                      calibration_attitude: DCM,
                      num_attitude_pairs: int) -> Tuple[Tuple[DCM, DCM], List[Tuple[DCM, DCM]]]:
    """Produces the attitudes to use with FOUND and tools.generator

    Args:
        local_attitude (Optional[DCM]): The local attitude to use in the first test pair
        calibration_attitude (DCM): The calibration rotation
        num_attitude_pairs (int): The number of test attitude pairs to generate

    Returns:
        Tuple[Tuple[Attitude, Attitude], List[Tuple[Attitude, Attitude]]]: A tuple of:
            1. The calibration attitudes to use (local, reference)
            2. A list of test attitudes to use (local, reference)
    """
    # Step 0: Validate the arguments
    validate_arguments(local_attitude, calibration_attitude, num_attitude_pairs)

    # Step 2: Produce the calibration bases
    reference_attitude_cal = DCM()
    local_attitude_cal = calibration_attitude.rotate(reference_attitude_cal)
    calibration_attitudes = local_attitude_cal, reference_attitude_cal
    
    # Step 3: Produce the Test Attitudes
    test_attitudes = list()
    
    # Step 3a: Handle the First Test Attitude Differently if we are given a local attitude
    loc_attitude = local_attitude
    ref_attitude = calibration_attitude.rotate(local_attitude, inverse=True)
    test_attitudes.append((loc_attitude, ref_attitude))
    
    # Step 3b: Handle the rest like as normal
    for i in range(num_attitude_pairs - 1):
        ref_attitude = DCM()
        loc_attitude = calibration_attitude.rotate(ref_attitude)
        
        test_attitudes.append((loc_attitude, ref_attitude))

    # Return the result
    return calibration_attitudes, test_attitudes

def output_result(calibration_attitude: DCM,
                  calibration_attitudes: Tuple[DCM, DCM],
                  test_attitudes: List[Tuple[DCM, DCM]]):
    """Outputs the results

    Args:
        calibration_attitude (DCM): The calibration rotation
        calibration_attitudes (Tuple[DCM, DCM]): The attitudes to use for calibration
        test_attitudes (List[Tuple[DCM, DCM]]): The attitudes to use for distance
    """
    # Define constants

    LF = "\n"
    SECTION_SEPERATOR = LF + LF
    SLASHLF = "\\" + LF
    
    # Define output functions
    
    def print_attitude(banner: str, attitude: Attitude, end=LF):
        if end is None:
            print(f"{banner:25}", f"RA: {attitude.ra:>7.3f}, DE: {attitude.de:>7.3f}, ROLL: {attitude.roll:>7.3f}")
        else:
            print(f"{banner:25}", f"RA: {attitude.ra:>7.3f}, DE: {attitude.de:>7.3f}, ROLL: {attitude.roll:>7.3f}", end)
    
    def print_attitudes(loc_attitude: Attitude, ref_attitude: Attitude):
        print_attitude(LOCAL_HEADER, loc_attitude, None)
        print_attitude(REFERENCE_HEADER, ref_attitude)
    
    def print_distance_commands(loc_attitude: Attitude, ref_attitude: Attitude):
        generator_cmd = f"Generator Command:{LF}tools.generator --position <Position> {SLASHLF}" \
            + f"--orientation {loc_attitude.to_generator_format()} {SLASHLF}" \
            + f"<camera_parameters> {SLASHLF}" \
            + f"--filename <generated_image_name>"
        distance_cmd = f"Distance Command:{LF}./build/bin/found distance {SLASHLF}" \
            + f"--calibration-data <calibration_file_name>.found {SLASHLF}" \
            + f"--reference-orientation {ref_attitude.to_found_format()} {SLASHLF}" \
            + f"<camera_parameters> {SLASHLF}" \
            + f"--image <generated_image_name>"

        print(generator_cmd, LF)
        print(distance_cmd)
    
    # Convert everything to Attitude
    calibration_attitude = calibration_attitude.to_attitude()
    calibration_attitudes = calibration_attitudes[0].to_attitude(), \
                            calibration_attitudes[1].to_attitude()
    for i in range(len(test_attitudes)):
        loc_attitude, ref_attitude = test_attitudes[i]
        test_attitudes[i] = loc_attitude.to_attitude(), ref_attitude.to_attitude()
    
    # Define Calibration Command
    CALIBRATION_COMMAND = f"Calibration Command:{LF}./build/bin/found calibration {SLASHLF}" \
        + f"--reference-orientation {calibration_attitudes[1].to_found_format()} {SLASHLF}" \
        + f"--local-orientation {calibration_attitudes[0].to_found_format()} {SLASHLF}" \
        + f"--output-file <calibration_file_name>.found"

    # Step 4: Output

    # Step 4a: Print Calibration Step
    print(f"{LF}{CALIBRATION_BANNER}", LF)
    print_attitude(CALIBRATION_HEADER, calibration_attitude)
    print_attitudes(*calibration_attitudes)
    print(CALIBRATION_COMMAND, SECTION_SEPERATOR)

    # Step 4b: Print all other Steps
    print(DISTANCE_BANNER, LF)
    for i, (loc_attitude, ref_attitude) in enumerate(test_attitudes):
        print(f"===== TEST PAIR {i + 1} =====", LF)
        print_attitudes(loc_attitude, ref_attitude)
        print_distance_commands(loc_attitude, ref_attitude)
        if i != len(test_attitudes) - 1:
            print(SECTION_SEPERATOR)
        else:
            print()
    
if __name__ == "__main__":
    arguments = parse_args()
    attitudes = produce_attitudes(*arguments)
    output_result(arguments[-2], *attitudes)