project( Coffee C CXX )
cmake_minimum_required( VERSION 3.1 )

# We set some general options for building, just to keep the binaries and libs in one place
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules )

# Make it verbose
set ( CMAKE_VERBOSE_MAKEFILE on )

# This is used inside modules to refer to libraries
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/libs )

include ( BuildOptions )

include ( BuildFunctions )

include ( BuildFlags )

# Primary includes
include_directories (
    ${CMAKE_SOURCE_DIR}
    include
    )

# General include directories, not ours
include_directories ( SYSTEM
    ${CMAKE_BINARY_DIR}/libs/include
    libs
    )

# Add automatic linking directory, useful to get builds up quickly
link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
    )

# Extension include directories
set (
    COFFEE_EXT_PCL_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/ext/pcl-shim/include
    )
set (
    COFFEE_EXT_FREENECT2_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/ext/kinect-shim/include
    ${COFFEE_CORE_INCLUDE_DIR}
    )

add_definitions( -DCOFFEE_APPLICATION_NAME="Generic Coffee" )
add_definitions( -DCOFFEE_ORGANIZATION_NAME="Coffee Guy" )
add_definitions( -DCOFFEE_VERSION_CODE=1 )

# Add subdir for core modules
add_subdirectory(coffee)

# Add subdir for example applications
add_subdirectory(examples)
# Add documentation directory, not built by default (increases build time)
add_subdirectory(docs)

# SWIG build, not functional anymore
if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(bindings/swig)
endif()

# Tools, includes the GUI profiler
add_subdirectory(tools)

# Add subdir for extensions, weak bindings of libraries
add_subdirectory(ext)
