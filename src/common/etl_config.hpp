#ifndef SRC_COMMON_ETL_CONFIG_HPP_
#define SRC_COMMON_ETL_CONFIG_HPP_

// Default max sizes for ETL containers (can be overridden by CMake or user)
#ifndef FOUND_MAX_POINTS
#define FOUND_MAX_POINTS 4096
#endif
#ifndef FOUND_MAX_IMAGE_PIXELS
#define FOUND_MAX_IMAGE_PIXELS 1048576
#endif
#ifndef FOUND_MAX_EDGES
#define FOUND_MAX_EDGES 1024
#endif
#ifndef FOUND_MAX_COMPONENTS
#define FOUND_MAX_COMPONENTS 1024
#endif
#ifndef FOUND_MAX_LOCATION_RECORDS
#define FOUND_MAX_LOCATION_RECORDS 4096
#endif

#endif  // SRC_COMMON_ETL_CONFIG_HPP_
