import unittest
from scipy.spatial.transform import Rotation

from src.transform.transform import Attitude, DCM
from src.common.constants import ROTATION_ORDER

class TransformTest(unittest.TestCase):
    
    def test_attitude_is_normalized_ra_above(self):
        attitude = Attitude(400, 40, -92)
        
        self.assertAlmostEqual(40, attitude.ra)
    
    def test_attitude_is_normalized_ra_below(self):
        attitude = Attitude(-40, 40, -92)
        
        self.assertAlmostEqual(320, attitude.ra)
    
    def test_attitude_is_normalized_de_above(self):
        attitude = Attitude(90, 270, 0)
        
        self.assertAlmostEqual(-90, attitude.de)
    
    def test_attitude_is_normalized_de_below(self):
        attitude = Attitude(90, -180 - 45, 0)
        
        self.assertAlmostEqual(135, attitude.de)
        
    def test_attitude_is_normalized_roll_above(self):
        attitude = Attitude(0, 90, 360 + 90)
        
        self.assertAlmostEqual(90, attitude.roll)
    
    def test_attitude_is_normalized_roll_below(self):
        attitude = Attitude(0, 90, -90)
        
        self.assertAlmostEqual(270, attitude.roll)
        
    def test_attitude_to_euler(self):
        attitude = Attitude(48, -26, 278)
        
        angles = attitude.to_dcm().rotation.as_euler(ROTATION_ORDER, degrees=True)
        actual = Attitude(angles[0], -angles[1], -angles[2])

        self.assertAlmostEqual(attitude.ra, actual.ra)
        self.assertAlmostEqual(attitude.de, actual.de)
        self.assertAlmostEqual(attitude.roll, actual.roll)
    
    def test_dcm_rotate_simple(self):
        dcm = DCM(Rotation.from_euler(ROTATION_ORDER, [90, 0, 0], degrees=True))
        
        input_angles = [86, -26, 92]
        input = DCM(Rotation.from_euler(ROTATION_ORDER, input_angles, degrees=True))
        
        actual = dcm.rotate(input).rotation.as_euler(ROTATION_ORDER, degrees=True)
        
        self.assertAlmostEqual(input_angles[0] + 90, actual[0])
        self.assertAlmostEqual(input_angles[1], actual[1])
        self.assertAlmostEqual(input_angles[2], actual[2])
    
    def test_dcm_rotate_simple_inverse(self):
        dcm = DCM(Rotation.from_euler(ROTATION_ORDER, [90, 0, 0], degrees=True))
        
        input_angles = [86, -26, 92]
        input = DCM(Rotation.from_euler(ROTATION_ORDER, input_angles, degrees=True))
        
        actual = dcm.rotate(input, inverse=True).rotation.as_euler(ROTATION_ORDER, degrees=True)
        
        self.assertAlmostEqual(input_angles[0] - 90, actual[0])
        self.assertAlmostEqual(input_angles[1], actual[1])
        self.assertAlmostEqual(input_angles[2], actual[2])
    
    def test_dcm_rotate_general(self):        
        for _ in range(10):
            rotation_angles = Rotation.random()
            input_angles = Rotation.random()
            
            rotation = DCM(rotation_angles)
            input = DCM(input_angles)
            
            expected_angles = list((rotation_angles * input_angles).as_euler(ROTATION_ORDER, degrees=True))
            actual_angles = list(rotation.rotate(input).rotation.as_euler(ROTATION_ORDER, degrees=True))
            
            self.assertListEqual(expected_angles, actual_angles)
            
            expected_angles_inv = list((rotation_angles.inv() * input_angles).as_euler(ROTATION_ORDER, degrees=True))
            actual_angles_inv = list(rotation.rotate(input, inverse=True).rotation.as_euler(ROTATION_ORDER, degrees=True))
            
            self.assertListEqual(expected_angles_inv, actual_angles_inv)