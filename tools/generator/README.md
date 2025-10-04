# Generator

The `generator` tool generates artificial images of earth using a position and orientation.

Required Flags:
1. `--position`: The position of the object, in meters (space seperated)
2. `--orientation`: The orientation of the object, in DE, RA, ROLL (space seperated)
Optional Flags
1. `--filename`: The name of the output file
2. For the rest, run `tools.generator --help`

Example Usage (from `found` root):
```bash

python3 -m tools.generator --position 10378137 0 0 --orientation 140 0 0 --focal-length 85e-3 --pixel-size 20e-6 --x-resolution 512 --y-resolution 512 --filename example_earth.png

```

![Example Image of Earth](../../test/common/assets/example_earth1.png)