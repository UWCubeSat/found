# FOUND: FOUND Open-Source Universal Navigation Determiner

FOUND is a system that Earth satellites can use to calculate their orbits using pictures taken by the satellite of Earth. It is being developed at Husky Satellite Lab, a CubeSat team at the University of Washington. It will be deployed and tested on HuskySat-2, the team's next mission.

# Installation

If you are using Windows, we recommend downloading Windows Subsystem for Linux, and following the instructions for Linux.

## Linux Prerequisites
FOUND runs on a Linux System that has many requirements. The following packages are required, with download instructions given in Ubuntu:
- Git (`sudo apt install git`)
- A C++ compiler (`sudo apt install g++`)
- GNU Make (`sudo apt install make`)
- CMake (`sudo apt install cmake`)
- Valgrind (`sudo apt install valgrind`)
- Python PIP, or more specifically, its cpplint file (`sudo apt install pip && pip install cpplint`)

You should natively have the following, but if not, the download instructions in Ubuntu are given for the following packages:
- wget (`sudo apt install wget`)
- tar (`sudo apt install tar`)

## macOS Prerequisites
You can also run FOUND on a macOS. To do so:
- Download Homebrew, run /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" in terminal

## Building FOUND
- Clone this repository (`git clone https://github.com/UWCubeSat/found.git`)
- Go into the directory (`cd found`)
- Compile the executable (`make`)
- Execute the executable (`./found`)

If you modify the local copy of this repository, only the last 2 instructions need to be repeated (unless you have `cd`'ed into another folder)


# Usage
FOUND is still in development! Come back in about 3 to 6 months to see how to run FOUND.

# Capabilities
FOUND will have the following components/modules, which all function together produce an orbital projection for a given satellite.

## Edge Detection
After images from a satellite are received, their images are parsed to locate Earth's horizon in the image. FOUND will be capable of:
- [ ] Simple Edge Detection via Simple Thresholding
- [ ] Laplacian of Gaussian (LoG) Filtered Edge Detection

## Distance Determination
The edge information is then used to evaluate the relative size of Earth in the image and find the distance of the satellite from Earth using principals of scale. FOUND will be capable of:
- [ ] Distance Determination with a Spherical Earth Assumption
- [ ] Distance Determination with an Ellipsoid Earth Assumption

## Vector Generation
The distance information will then be used to form a vector of the satellite relative to Earth's coordinate axes. FOUND will be capable of:
- [ ] Star-Tracker Assisted Vector Generation via LOST
- [ ] Feature Detection Assisted Vector Generation (to be developed for future mission)

## Orbit Determination
This stage takes multiple vectors of the satellite at different points in the satellite's orbit to project the satellite's future path of travel. FOUND will be capable of:
- [ ] Stable Elliptical Orbit Determination
- [ ] Preceding Elliptical Orbit Determination

## Kinematic Profiling
This stage then takes the projected path of travel and matches it to the speed of the satellite, providing the position and velocity vectors of the satellite at any future time. FOUND will be capable of:
- [ ] Eulerian-based Kinematic Profiling
- [ ] Keplerian-based Kinematic Profiling