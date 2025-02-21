import unittest

from common.constants import EARTH_RADIUS


class IntegrationTest(unittest.TestCase):

    # Taking an inductive approach to this. First we manually calculate the circle we expect to see looking at the image head on, then we take those generated pixels that we know are correct, and then rotate them to get the other rotated images. Because we know our base pixels are correct and our rotation algorithm, then we can use them without worrying too much.
    pass
