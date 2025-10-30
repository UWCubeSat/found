from scipy.spatial.transform import Rotation
from common.constants import ROTATION_ORDER, ANGLE_NORM

class Attitude:
    """Represents an attitude in the celestial sphere (using modified Euler Angles)
    
    Note:
        We use degrees here
    """

    def __init__(self, ra: float, de: float, roll: float): # pyright: ignore[reportRedeclaration]
        """Creates an Attitude object

        Args:
            right_ascension (float): The right ascension of the attitude
            declination (float): The declination of the attitude
            roll (float): The roll of the attitude
        """
        self.ra = ra
        self.de = de
        self.roll = roll

        while self.ra > ANGLE_NORM:
            self.ra -= ANGLE_NORM
        while self.ra < 0:
            self.ra += ANGLE_NORM

        while self.de > ANGLE_NORM / 2:
            self.de -= ANGLE_NORM
        while self.de < -ANGLE_NORM / 2:
            self.de += ANGLE_NORM

        while self.roll > 0:
            self.roll -= ANGLE_NORM
        while self.roll < 0:
            self.roll += ANGLE_NORM

    def to_dcm(self):
        """Converts the Attitude to a DCM

        Returns:
            DCM: The resulting DCM
        """
        return  DCM(Rotation.from_euler(ROTATION_ORDER, [self.ra, -self.de, -self.roll], degrees=True))

    def to_generator_format(self) -> str:
        """Provides the string representation for the tools.generator CLI

        Returns:
            str: The string to pass to the tools.generator CLI
        """
        return f"{self.ra} {self.de} {self.roll}"

    def to_found_format(self) -> str:
        """Provides the string representation for the FOUND CLI

        Returns:
            str: The string to pass to the FOUND CLI
        """
        return f"\"{self.to_generator_format()}\""

class DCM:
    """Represents a Rotation/Orthonormal Basis
    """
    
    def __init__(self, rotation: Rotation = None):
        """Constructs a DCM from a scipy Rotation

        Args:
            rotation (Rotation, optional): The rotation to use. Defaults to Rotation.random().
        """
        if rotation is None:
            rotation = Rotation.random()
        self.rotation = rotation

    def rotate(self, other, inverse: bool = False):
        """Rotates another DCM

        Args:
            other (DCM): The other DCM to rotate
            inverse (bool, optional): Whether to apply the inverse rotation. Defaults to False.

        Returns:
            DCM: The resulting DCM
        """
        if not inverse:
            return DCM(self.rotation * other.rotation)
        return DCM(self.rotation.inv() * other.rotation)

    def to_attitude(self) -> Attitude:
        """Represents the DCM as an Attitude basis

        Returns:
            Attitude: The attitude represented by this basis
        """
        ra, de, roll = self.rotation.as_euler(ROTATION_ORDER, degrees=True)
        return Attitude(ra, -de, -roll)