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

python -m tools.generator  --position 10000000 0 0 --orientation 180 0 0

```