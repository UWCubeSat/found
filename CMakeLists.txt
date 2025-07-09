include(FetchContent)
include(ExternalProject)

########## Utility Functions ##########

# Checks if the program exists and has correct version #
function(check_program_version out_var program_name required_version)
    # Find the program
    find_program(PROGRAM_PATH_${out_var} ${program_name})
    if(NOT PROGRAM_PATH_${out_var})
        message(FATAL_ERROR "${program_name} not found")
    endif()

    # Get the version string (adjust the flag if needed)
    execute_process(
        COMMAND ${PROGRAM_PATH_${out_var}} --version
        OUTPUT_VARIABLE VERSION_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Extract the version number (first x.y or x.y.z match)
    string(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?" VERSION_EXTRACTED
                 "${VERSION_RAW}")
    if(VERSION_EXTRACTED VERSION_LESS ${required_version})
        message(
            FATAL_ERROR
                "${program_name} version ${required_version} or higher required, but found ${VERSION_EXTRACTED}"
        )
    endif()

    # Return both the path and version
    set(${out_var}
        ${PROGRAM_PATH_${out_var}}
        PARENT_SCOPE)
endfunction()

########## CMake Project Requirements ##########

cmake_minimum_required(VERSION 3.16)
project(found LANGUAGES CXX)

########## File/Folder Definitions ##########

# Build directories
set(BUILD_DIR ${CMAKE_BINARY_DIR})
set(CACHE_DIR ${CMAKE_SOURCE_DIR}/.cache)
set(BIN_DIR ${BUILD_DIR}/bin)
# set(OBJ_DIR ${BUILD_DIR}/objects) No longer needed
set(DOC_DIR ${BUILD_DIR}/documentation)

# Source directory
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SRC ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE SRC_H ${SRC_DIR}/*.hpp)
set(FILTERED_SRC ${SRC})
list(FILTER FILTERED_SRC EXCLUDE REGEX ".*src/main\\.cpp$")
set(SRC_MAIN "${SRC_DIR}/main.cpp")

# Test directory
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)
file(GLOB_RECURSE TEST CONFIGURE_DEPENDS ${TEST_DIR}/*.cpp)
list(APPEND TEST ${FILTERED_SRC})
file(GLOB_RECURSE TEST_H ${TEST_DIR}/*.hpp)
list(APPEND TEST_H ${SRC_DIR})

# Documentation directory
set(DOC_COVERAGE_DIR ${DOC_DIR}/coverage)
set(DOC_DOXYGEN_DIR ${DOC_DIR}/doxygen)

########## C++ Shared Compilation Flags ##########

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include dirs
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${SRC_DIR})
include_directories(${CACHE_DIR})

# Logging macros
option(DISABLE_LOGGING "Disable logging" OFF)
set(LOGGING_LEVEL
    INFO
    CACHE STRING "Logging level")

# Floating-point config
option(FLOAT_MODE "Enable FOUND_FLOAT_MODE" OFF)
if(FLOAT_MODE)
    add_compile_definitions(FOUND_FLOAT_MODE)
endif()

# Debug config
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -fno-omit-frame-pointer")

# Release config
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Global compile options
add_compile_options(-Wall -Wextra -Wno-missing-field-initializers -pedantic)

########## Source Libraries ##########

# We compile all libraries as static libraries

##### stb_image #####

# stb_image definitions
set(STB_IMAGE "stb_image")
set(STB_IMAGE_URL
    "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DIR "${CACHE_DIR}/${STB_IMAGE}")
set(STB_IMAGE_HEADER "${STB_IMAGE_DIR}/${STB_IMAGE}.h")
set(STB_IMAGE_SRC "${STB_IMAGE_DIR}/${STB_IMAGE}.cpp")

# Setup directory for content
file(MAKE_DIRECTORY ${STB_IMAGE_DIR})
FetchContent_Declare(
    ${STB_IMAGE}
    URL https://raw.githubusercontent.com/nothings/stb/master/${STB_IMAGE}.h
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

# Download stb_image and compile into library
if(NOT EXISTS "${STB_IMAGE_HEADER}")
    message(STATUS "Downloading ${STB_IMAGE}")
    file(DOWNLOAD ${STB_IMAGE_URL} ${STB_IMAGE_HEADER})
endif()
if(NOT EXISTS ${STB_IMAGE_SRC})
    file(WRITE ${STB_IMAGE_SRC} "#include \"${STB_IMAGE}/${STB_IMAGE}.h\"\n")
endif()
add_library(${STB_IMAGE} STATIC ${STB_IMAGE_SRC})
target_compile_definitions(${STB_IMAGE} PRIVATE STB_IMAGE_IMPLEMENTATION)

# Set the source libraries
set(SRC_LIBS ${STB_IMAGE})

########## Test Libraries ##########

##### Googletest #####

set(GTEST googletest)
set(GTEST_VERSION release-1.12.1)
set(GTEST_DIR ${CACHE_DIR}/${GTEST}-${GTEST_VERSION})
set(GTEST_INSTALL_DIR ${GTEST_DIR}/src/${GTEST}-build/install)
set(GTEST_LIBS gtest gtest_main gmock gmock_main)

ExternalProject_Add(
    ${GTEST}
    GIT_REPOSITORY https://github.com/google/${GTEST}.git
    GIT_TAG ${GTEST_VERSION}
    PREFIX ${GTEST_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=install -DCMAKE_RULE_MESSAGES=OFF
               -DCMAKE_INSTALL_MESSAGE=NEVER
    UPDATE_COMMAND ""
    TEST_COMMAND ""
    BUILD_ALWAYS OFF
    LOG_CONFIGURE OFF
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

foreach(lib ${GTEST_LIBS})
    add_library(${lib} STATIC IMPORTED GLOBAL)
    set_target_properties(
        ${lib} PROPERTIES IMPORTED_LOCATION "${GTEST_INSTALL_DIR}/lib/lib${lib}.a")
    add_dependencies(${lib} ${GTEST})
endforeach()

set(TEST_LIBS ${GTEST_LIBS} ${SRC_LIBS})

##### Documentation Libraries #####

set(DOXYGEN_AWESOME doxygen-awesome-css)
set(DOXYGEN_AWESOME_VERSION 2.3.4)
set(DOXYGEN_AWESOME_URL
    https://github.com/jothepro/${DOXYGEN_AWESOME}/archive/refs/tags/v${DOXYGEN_AWESOME_VERSION}.tar.gz
)
set(DOXYGEN_AWESOME_ZIP ${CACHE_DIR}/v${DOXYGEN_AWESOME_VERSION}.tar.gz)
set(DOXYGEN_AWESOME_ARTIFACT
    ${CACHE_DIR}/${DOXYGEN_AWESOME}-${DOXYGEN_AWESOME_VERSION})

add_custom_command(
    OUTPUT ${DOXYGEN_AWESOME_ARTIFACT}
    COMMAND wget ${DOXYGEN_AWESOME_URL} -P ${CACHE_DIR}
    COMMAND tar -xzf ${DOXYGEN_AWESOME_ZIP} -C ${CACHE_DIR}
    VERBATIM
    COMMENT "Downloads doxygen-awesome")

########## Targets ##########

add_compile_options(-Wdouble-promotion -Werror)

##### compile #####

file(MAKE_DIRECTORY ${BIN_DIR})
add_executable(found ${SRC})
target_link_libraries(found PUBLIC ${SRC_LIBS})
if(NOT DISABLE_LOGGING)
    target_compile_definitions(found PRIVATE ENABLE_LOGGING
                                             LOGGING_LEVEL=${LOGGING_LEVEL})
endif()
set_target_properties(found PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_custom_target(compile
                  DEPENDS found
                  COMMENT "Compiles source code")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")

    ##### test #####

    add_executable(found-test EXCLUDE_FROM_ALL ${TEST})
    target_include_directories(found-test
                               PRIVATE "${GTEST_INSTALL_DIR}/include")
    target_compile_definitions(found-test PRIVATE ENABLE_LOGGING
                                                  LOGGING_LEVEL=INFO)
    target_compile_options(found-test PRIVATE --coverage -pthread)
    target_link_libraries(found-test PRIVATE --coverage)
    if(NOT OMIT_ASAN)
        target_compile_options(found-test PRIVATE -fsanitize=address
                                                  -fomit-frame-pointer)
        target_link_libraries(found-test PRIVATE -fsanitize=address
                                                 -fomit-frame-pointer)
    endif()
    target_link_libraries(found-test PRIVATE ${TEST_LIBS})
    set_target_properties(found-test PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
    add_dependencies(found-test ${GTEST} ${TEST_LIBS})

    ##### coverage #####

    file(MAKE_DIRECTORY ${DOC_COVERAGE_DIR})

    check_program_version(GCOVR gcovr 8.3)
    set(GCOVR_CONFIG ${CMAKE_SOURCE_DIR}/gcovr.cfg)

    add_custom_target(
        coverage
        COMMAND cd ${CMAKE_SOURCE_DIR} && ${BIN_DIR}/found-test --gtest_brief=1
        COMMAND ${GCOVR} -r ${CMAKE_SOURCE_DIR}
        # WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running tests and generating coverage report"
        VERBATIM
        DEPENDS found-test)

    ##### linting #####

    check_program_version(CPPLINT cpplint 2.0.0)

    add_custom_target(
        lint
        COMMAND ${CPPLINT} --recursive ${SRC_DIR} ${TEST_DIR}
        DEPENDS ${SRC} ${TEST}
        COMMENT "Checks Google C++ Style on Code")

    ##### documentation #####

    find_package(Doxygen 1.9.8 REQUIRED)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required but was not found.")
    endif()

    add_custom_target(
        documentation
        COMMAND ./doxygen.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
        DEPENDS ${DOXYGEN_AWESOME_ARTIFACT} ${SRC}
        COMMENT "Generates Doxygen Documentation over code")

endif()

##### clean_all #####

add_custom_target(clean_all
                  COMMAND rm -rf ${CACHE_DIR} ${BUILD_DIR}
                  COMMENT "Cleans the build and cache folders")

##### default target (does not run in order) #####
add_custom_target(default ALL
                  COMMENT "The default target")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_dependencies(default documentation)
    add_dependencies(documentation lint)
    add_dependencies(lint coverage)
    add_dependencies(coverage compile)
else()
    # We only install during release
    add_dependencies(default compile)
    install(TARGETS found DESTINATION bin)
    install(
        DIRECTORY ${SRC_DIR}/
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.hpp")
endif()
