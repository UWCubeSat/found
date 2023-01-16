# FOUND
FOUND: FOUND Open-Source Universal Navigation Determiner

Feel free to add to everything. We want to track as many requirements/steps as possible so there is less cleanup work.

## Requirements
- [ ] Accurately Model Satellite Orbit
  - [ ] Accurately predict Position above Earth (or other celestial body) at any point in orbit
  - [ ] Accurately predict the velocity at any point in orbit
- [ ] Runtime about 3 seconds maximum to generate an accurate kinematic profile for the satellite.
- [ ] Use of Embedded-Friendly Data Structures (Use as little space as possible)

## Design

### Finding Position Above Earth (Algorithms names to be inserted)

1. Edge Detection
The first part of the FOUND will take a picture of Earth and locate all points along the horizon.

Note that points must be filtered and enhanced before edge detection begins. These are edge cases, so they do not need to be immediately handled.
- [ ] Laplacian of Gaussian (This does all three steps at once)

2. Curve Fitting
The second part will take all points on the horizon to model the elliptical shape of the earth.

The equation is non linear, so there are no convient linear algorithms, but here are some ideas to get you started.
- [ ] Gaussian Elimination
- [ ] Gauss-Siedel/Jacobain
- [ ] Least Square-Sum Error
- [ ] [Non-linear regression](https://corporatefinanceinstitute.com/resources/data-science/nonlinear-regression/)

3. Position Determination
The part takes the size derived from the curve fit and determines the distance from earth. This is can be derived from photogrammetry. I found a [paper](https://www.lpl.arizona.edu/hamilton/sites/lpl.arizona.edu.hamilton/files/courses/ptys551/Principles_of_Photogrammetry.pdf.) that may be helpful.

Keep in mind that distortion due to camera lens and atmospheric distortion will be present, but we can handle this in steps before the main algorithm that finds the position.

4. Position Vector Output
This last part takes the previous derived information and outputs a spatial vector relative to Earth's center. The vector for now will be in astronomical coordinates (dec, ra, radius), and its fairly easy to convert to cartesian or some other form if needed. To obtain orientation information

- [ ] Feature Detection

or

- [ ] Take information from LOST


### Finding Orbit/Velocity

For now, assume that the orbital system is 2-body, since we extract no other information than the one pertaining to Earth, and that Earth exerts the strongest gravitational pull experienced by the satellite.

1. Repeat of Previous Process (Finding Position Above Earth) with time delay
This is to get a second data point to define the orbital system.

2. Find velocity using Euler Approximation
This is just a simple slope equation, but the next part may eliminate the need for this.

3. Find the orbital path of the satellite

This can be done in two ways:
- Fit position and velocity information to orbit position equation
- Fit both positions to an eliptical equation about the barrycenter of the earth-satellite system (Probably just the center of earth).


The output of this entire stage should be constants relavent to the eliptical equation.
