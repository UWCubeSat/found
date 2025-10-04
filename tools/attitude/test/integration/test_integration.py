import unittest
import re
import io
from contextlib import redirect_stdout
from scipy.spatial.transform import Rotation
from typing import Optional

from src.transform.transform import DCM, Attitude
from src.common.constants import (
    ROTATION_ORDER,
    CALIBRATION_BANNER,
    DISTANCE_BANNER,
    CALIBRATION_HEADER,
    LOCAL_HEADER,
    REFERENCE_HEADER
)

from src.__main__ import produce_attitudes, output_result

RE_FLOAT = r"[+-]?\d+(?:\.\d+)?"
RE_WHITESPACE = r"\s+"

DEFAULT_NUM_POINTS = 50

class Arguments:
        def __init__(self, use_local: bool,
                     local_attitude: Optional[DCM],
                     calibration_attitude: DCM,
                     num_attitude_pairs: int = DEFAULT_NUM_POINTS):
            self.use_local = use_local
            self.local_attitude = local_attitude
            self.calibration_attitude = calibration_attitude
            self.num_attitude_pairs = num_attitude_pairs
        def to_tuple(self):
            return self.use_local, self.local_attitude, self.calibration_attitude, self.num_attitude_pairs

class IntegrationTest(unittest.TestCase):      
    
    def assertAttitudesAlmostEqual(self, att1: Attitude, att2: Attitude):
        self.assertAlmostEqual(att1.ra, att2.ra, 2)
        self.assertAlmostEqual(att1.de, att2.de, 2)
        self.assertAlmostEqual(att1.roll, att2.roll, 2)
    
    def assertDCMAlmostEqual(self, dcm1: DCM, dcm2: DCM):
        self.assertAttitudesAlmostEqual(dcm1.to_attitude(), dcm2.to_attitude())
    
    def assertAttitudeOffsetRA(self, dcm1: DCM, dcm2: DCM, offset: float):
        att1, att2 = dcm1.to_attitude(), dcm2.to_attitude()

        mod2 = Attitude(att2.ra + offset, att2.de, att2.roll)

        self.assertAttitudesAlmostEqual(att1, mod2)
    
    def assertAttitudeOffsetGeneral(self, dcm1: DCM, dcm2: DCM, offset: DCM):
        self.assertDCMAlmostEqual(dcm1, offset.rotate(dcm2))

    def test_no_rotation(self):
        arguments = Arguments(False, None, DCM(Rotation.from_euler(ROTATION_ORDER, [0, 0, 0], degrees=True)))
        
        calibration, tests = produce_attitudes(*arguments.to_tuple())
        
        self.assertDCMAlmostEqual(*calibration)        
        
        for local, reference in tests:
            self.assertDCMAlmostEqual(local, reference)
    
    def test_simple_rotation(self):
        arguments = Arguments(False, None, DCM(Rotation.from_euler(ROTATION_ORDER, [135, 0, 0], degrees=True)))
        
        calibration, tests = produce_attitudes(*arguments.to_tuple())
        
        self.assertAttitudeOffsetRA(*calibration, 135)
        
        for local, reference in tests:
            self.assertAttitudeOffsetRA(local, reference, 135)
    
    def test_rotation_general(self):
        arguments = Arguments(False, None, DCM())
        
        calibration, tests = produce_attitudes(*arguments.to_tuple())
        
        self.assertAttitudeOffsetGeneral(*calibration, arguments.calibration_attitude)
        
        for local, reference in tests:
            self.assertAttitudeOffsetGeneral(local, reference, arguments.calibration_attitude)
    
    def test_rotation_general_with_local_attitude(self):
        local_attitude = DCM()
        arguments = Arguments(True, local_attitude, DCM())
        
        calibration, tests = produce_attitudes(*arguments.to_tuple())
        
        self.assertAttitudeOffsetGeneral(*calibration, arguments.calibration_attitude)
        
        for local, reference in tests:
            self.assertAttitudeOffsetGeneral(local, reference, arguments.calibration_attitude)
        
        self.assertDCMAlmostEqual(local_attitude, tests[0][0])
    
    def test_print_output(self):
        # Here, we don't actually care about the content of these
        calibration_attitude = DCM()
        calibration_attitudes = DCM(), DCM()
        test_attitudes = [(DCM(), DCM()) for _ in range(DEFAULT_NUM_POINTS)]
        
        attitude_list = [calibration_attitude, *calibration_attitudes, calibration_attitudes[1], calibration_attitudes[0]]
        for el in test_attitudes:
            attitude_list.extend(el)
            attitude_list.extend(el)
        
        common_matcher = rf"{RE_WHITESPACE}RA:{RE_WHITESPACE}({RE_FLOAT}),?{RE_WHITESPACE}DE:{RE_WHITESPACE}({RE_FLOAT}),?{RE_WHITESPACE}ROLL:{RE_WHITESPACE}({RE_FLOAT})"
        number_list_matcher = rf"({RE_FLOAT}) ({RE_FLOAT}) ({RE_FLOAT})"
        calibration_matcher = re.compile(rf"^{CALIBRATION_HEADER}{common_matcher}")
        ref_ori_matcher = re.compile(rf'^--reference-orientation "{number_list_matcher}"')
        local_ori_matcher = re.compile(rf'^--local-orientation "{number_list_matcher}"')
        pair_matcher = re.compile(rf"^{LOCAL_HEADER}{common_matcher}"), re.compile(rf"^{REFERENCE_HEADER}{common_matcher}")
        ori_matcher = re.compile(rf'^--orientation {number_list_matcher}')
        
        matcher_list = [calibration_matcher, *pair_matcher, ref_ori_matcher, local_ori_matcher]
        for _ in range(DEFAULT_NUM_POINTS):
            matcher_list.extend(pair_matcher)
            matcher_list.append(ori_matcher)
            matcher_list.append(ref_ori_matcher)
        
        f = io.StringIO()
        with redirect_stdout(f):
            output_result(calibration_attitude, calibration_attitudes, test_attitudes)
        
        lines = f.getvalue().splitlines()
        
        i = 0
        for line in lines:
            match = matcher_list[i].match(line)
            if match:
                self.assertDCMAlmostEqual(attitude_list[i], Attitude(*[float(num) for num in match.groups()]).to_dcm())
                i += 1
                if i == len(matcher_list):
                    break
        
        self.assertEqual(i, len(matcher_list))