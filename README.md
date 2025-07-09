# FOUND: Open-Source Universal Navigation Determiner

FOUND is a system that Earth satellites can use to calculate their orbits using pictures taken by the satellite of Earth. It is being developed at Husky Satellite Lab, a CubeSat team at the University of Washington. It will be deployed and tested on HuskySat-2, the team's next mission.

# Installation

If you are using Windows, you must download Windows Subsystem for Linux, and use a Linux distribution (Ubuntu by default) to run the following
code.

## Prerequisites
For Linux Ubuntu/Oracle and MacOS (uses either `apt-get`, `yum` or `brew`) obtain the required packages for this software:
- Copy the install script into your environment: [Link to install script](https://github.com/UWCubeSat/found/blob/main/install.sh)
- Change permissions for the install script to execute (`sudo chmod +rwx install.sh`)
- Run the install script in root/sudo as an executable (`sudo ./install.sh`)

## Building FOUND
- Clone the repository (`git clone https://github.com/UWCubeSat/found.git`)
- Go into the directory (`cd found`)
- Compile the executable via GNU Make or CMake (below for more information)
- Execute the executable (`./build/bin/found`)
- Execute the test suite (`./build/bin/found-test`)

This repository uses a dual build system, using GNU Make and CMake. As usual if your code changes, you must remake your file. The `build.sh` script abstracts the difference for you. It is run as:

1. Run GNU Make: `./build.sh make [GNU Make options]`
2. Run CMake: `./build.sh cmake "[CMake Configuration Options]" "[CMake Build Options]"`
3. Clean the build folder: `./build.sh clean`
4. Clean the build and cache folders: `./build.sh clean_all`

# Usage
The main executable, which is found in `./build/bin/found`, operates as a standard command-line based program. The usage is:
<div align="center">

`./build/bin/found <option> [flags]`

</div>
There are currently two options:

1. `calibration`: FOUND needs to know its own camera attitude, but does not find that information. Thus, this step produces a relative attitude based on a reference attitude that is always known, and one measurement of the FOUND camera's attitude. The output is to a `.found` file that stores this information.
2. `distance`: FOUND then uses an image taken from space together with a `.found` file to figure out the position of the image, and hence the satellite, relative to the Celestial Coordinate System (conversion to lattitude/longitude/altitude is also possible).

To learn more about the different flags, execute `./build/bin/found --help` or `./build/bin/found -h`.

## Example Usage
Here, we present a common usage for this program. After generating the binary via `make` or simply `make compile`, execute the command:

```bash
./build/bin/found calibration         \
    --reference-orientation "20,0,0"  \
    --local-orientation "50,0,0"      \
    --output-file "example-df.found"
```
Here's what the flags mean:
1. A reference attitude is given (degrees) in terms of right ascension, declination and roll.
2. Another attitude is given for the local orientation, which is the orientation of FOUND.
3. There is an output file to put our calibrational data in.

You will notice in the your current directory the file `example-df.found` has now appeared. You cannot directly read this file, but in particular, this file records a required Z rotation of -30 degrees to go from the reference to FOUND's attitude.

We can then use this calibration information to now make some position estimates of our satellite. Execute the command:

```bash
./build/bin/found distance                         \
    --image test/common/assets/example_earth1.png  \
    --calibration-data example-df.found            \
    --camera-focal-length 85e-3                    \
    --camera-pixel-size 20e-6                      \
    --reference-orientation 190,0,0
```

For the distance flags:
1. The image is just the path to the image we want to use. In this case, we reference an image we use in our test cases
2. We then specify our calibration data, which we generated above
3. We can specify camera parameters, like the focal length (m)
4. Or the pixel size (m)
5. We then have our reference orientation, which we need to use in conjunction with our calibration data

Feeding all this information, the program now analyzes the image, the calibration data and the rest of the parameters to give us its estimate, which is seen below:

```text
[INFO 2025-06-11 20:58:06 PDT] Using DataFile for calibration information
[INFO 2025-06-11 20:58:06 PDT] Calculated Position: (1.0456e+07, -67903.8, -972.935) m
[INFO 2025-06-11 20:58:06 PDT] Distance from Earth: 1.04562e+07 m
```

This is close to the actual position, which is `{10378137, 0, 0} m`. To be specific, the difference is 0.995379% in magnitude and 1339.65 arcsec in direction (this is especially good since we're using low quality edge detection).

Another thing to point out is the reference orientation, which was `{190,0,0}`. The image specified in the command was taken at an attitude of `{140,0,0}`, which if you remember, `{190,0,0}-{30,0,0}={140,0,0}` (this doesn't general hold, since attitude "subtraction" doesn't actually look like this). This of course is important to specify, otherwise, you'll get the same magnitude, but wrong direction. Should you not want to use the calibration file to do this odd math, you can instead do the equivalent command:

```bash
./build/bin/found distance                           \
    --image "test/common/assets/example_earth1.png"  \
    --reference-as-orientation                       \
    --camera-focal-length 85e-3                      \
    --camera-pixel-size 20e-6                        \
    --reference-orientation "140,0,0"
```
Where `--reference-as-orientation` tells the program to use whatever `--reference-orientation` is as the orientation of FOUND.

Lastly, one thing you will not notice is that the `temp.found` file has changed. In addition to storing calibration infomration, it also stores the calculated positions and their time stamps. This is important for the `orbit` stage, which is currently not implemented. If you should not want to output to the same file, you can instead do:

```bash
./build/bin/found distance                         \
    --image test/common/assets/example_earth1.png  \
    --calibration-data example-df.found            \
    --camera-focal-length 85e-3                    \
    --camera-pixel-size 20e-6                      \
    --reference-orientation 190,0,0                \
    --output-file other.found
```


# Capabilities
FOUND currently has the following components/modules, which all function together produce an position estimates and orbital projections for a given satellite.

## Calibration
This is a "simplistic" algorithm, and simply figures out the relative attitude between our local and reference attitude. The main algorithm responsible for this is:
- [X] LOST-based Calibration Algorithm (LOST is the reference)

## Distance Determination
This algorithm is comprised of 3 main steps

### Edge Detection
After images from a satellite are received, their images are parsed to locate Earth's horizon in the image. FOUND is capable of:
- [X] Simple Edge Detection via Space/Ether Identification
- [ ] Laplacian of Gaussian (LoG) Filtered Edge Detection

### Distance Determination
The edge information is then used to evaluate the relative size of Earth in the image and find the distance of the satellite from Earth using principals of scale. FOUND is capable of:
- [X] Simple Spherical Planet Distance Determination
- [X] Iterative Spherical Planet Distance Determination with SoftMax Activation and Selective Randomization
- [ ] Distance Determination with an Ellipsoid Planet Assumption

### Vector Generation
The distance information will then be used to form a vector of the satellite relative to Earth's coordinate axes. FOUND is capable of:
- [X] Star-Tracker Assisted Vector Generation via LOST-based Calibration
- [ ] Feature Detection Assisted Vector Generation (to be developed for future mission)

## Orbit Determination
This stage takes multiple vectors of the satellite at different points in the satellite's orbit to project the satellite's future path of travel. FOUND will be capable of:
- [ ] Runge-Kutta Based Orbital Prediction
- [ ] Lambert's Problem-Based Orbital Prediction

# CMake Integration

This project's code can be integrated with yours via cmake. To use:
```CMake
include(FetchContent)

cmake_minimum_required(VERSION 3.16)
project(test)

FetchContent_Declare(
  found
  GIT_REPOSITORY https://github.com/UWCubeSat/found.git
)

FetchContent_MakeAvailable(found)

add_executable(your_executable ...)
target_link_libraries(test PRIVATE found::found_lib) # Link our library
```
The library `found::found_lib` already has all the include statements needed inside of it, so no need to locate them.