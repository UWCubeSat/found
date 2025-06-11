# FOUND: FOUND Open-Source Universal Navigation Determiner

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
- Compile the executable (`make`)
- Execute the executable (`./build/bin/found`)
- Execute the test suite (`./build/bin/found-test`)

As this repository uses GNU Make to generate its artifacts, you'll need to rerun `make` everytime you change the code.

# Usage
FOUND is still in development! Come back in about 3 to 6 months to see how to run FOUND.

# Capabilities
FOUND will have the following components/modules, which all function together produce an orbital projection for a given satellite.

## Edge Detection
After images from a satellite are received, their images are parsed to locate Earth's horizon in the image. FOUND will be capable of:
- [X] Simple Edge Detection via Space/Ether Identification
- [ ] Laplacian of Gaussian (LoG) Filtered Edge Detection

## Distance Determination
The edge information is then used to evaluate the relative size of Earth in the image and find the distance of the satellite from Earth using principals of scale. FOUND will be capable of:
- [X] Distance Determination with a Spherical Earth Assumption
- [ ] Distance Determination with an Ellipsoid Earth Assumption

## Vector Generation
The distance information will then be used to form a vector of the satellite relative to Earth's coordinate axes. FOUND will be capable of:
- [X] Star-Tracker Assisted Vector Generation via LOST
- [ ] Feature Detection Assisted Vector Generation (to be developed for future mission)

## Orbit Determination
This stage takes multiple vectors of the satellite at different points in the satellite's orbit to project the satellite's future path of travel. FOUND will be capable of:
- [ ] Runge-Kutta Based Orbital Prediction
- [ ] Lambert's Problem-Based Orbital Prediction