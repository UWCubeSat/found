#include "distance/vectorize.hpp"

#include <math.h>
#include <assert.h>
#include <iostream>

#include "spatial/attitude-utils.hpp"  // Import for vector math
#include "style/style.hpp"

namespace found {

//////////////////////////////////////////////////
//////// VectorGenerationAlgorithm CLASS /////////
//////////////////////////////////////////////////
VectorGenerationAlgorithm::~VectorGenerationAlgorithm() {}

//////////////////////////////////////////////////
////// LOSTVectorGenerationAlgorithm CLASS ///////
//////////////////////////////////////////////////

// constructor
LOSTVectorGenerationAlgorithm::LOSTVectorGenerationAlgorithm(Attitude refAttitude, Attitude difference)
      : refAttitude(refAttitude), difference(difference) {}

// Destructor
LOSTVectorGenerationAlgorithm::~LOSTVectorGenerationAlgorithm() {}

// Gets position vector from Earth in celestial coordinate frame
PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector& v_E) {
      // Extract Euler angles from attitudes
      EulerAngles ref_euler = refAttitude.ToSpherical();
      EulerAngles dif_euler = difference.ToSpherical();
      // Combine angles to get quaternion
      decimal ra_combined = ref_euler.ra + dif_euler.ra;
      decimal de_combined = ref_euler.de + dif_euler.de;
      decimal roll_combined = ref_euler.roll + dif_euler.roll;
      // Create transformer
      Quaternion foundTransformer = SphericalToQuaternion(ra_combined, de_combined, roll_combined);
      // Update the vector to have Earth at the center. Flipping the sign does the trick
      PositionVector earthToSat = v_E*(-1);
      return foundTransformer.Rotate(earthToSat);  // Vector from earth to the satellite in the correct frame
}

///////////////////////////////////////////////////////
////// FeatureDetectionVectorGenerationAlgorithm //////
///////////////////////////////////////////////////////

// TODO: IMPLEMENT

}  // namespace found
