project( Coffee C CXX )
cmake_minimum_required( VERSION 3.1 )

# Make it verbose
set ( CMAKE_VERBOSE_MAKEFILE on )

include ( CTest )

set ( CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/Find
    ${CMAKE_SOURCE_DIR}/cmake/Modules
    ${CMAKE_SOURCE_DIR}/cmake/Packaging
    )

if(WIN32)
    # FindOpenSSL is broken on Windows.
    # There is this 'replacement' that works in our environment.
    set ( CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake/Find_Windows
        ${CMAKE_MODULE_PATH}
        )
endif()


# We set some general options for building, just to keep the binaries and libs in one place
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
if(NOT WIN32)
    set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
else()
    set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
endif()
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

# Custom, Coffee-specific variables
set ( CMAKE_PACKAGED_OUTPUT_PREFIX packaged ) # Custom, used by scripts

set ( COFFEE_DESKTOP_DIRECTORY "${CMAKE_SOURCE_DIR}/desktop" )
set ( COFFEE_ASSET_DIRECTORY "${CMAKE_SOURCE_DIR}/sample_data" )

set ( COFFEE_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_PACKAGED_OUTPUT_PREFIX}" )
set ( COFFEE_DEPLOY_DIRECTORY "${PROJECT_BINARY_DIR}/deploy" )
set ( COFFEE_SOURCE_TEMPLATE_DIRECTORY "${CMAKE_SOURCE_DIR}/internal/templates" )

set ( COFFEE_NATIVE_LIB_DIR CACHE PATH "" )

set ( COFFEE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/coffee" )
set ( COFFEE_PCH_DIR "${CMAKE_BINARY_DIR}/pch/" )

if(COFFEE_GENERATE_LIBRARIES)
    message (STATUS "Building as: ${CMAKE_BUILD_TYPE}")
endif()

# This is used inside modules to refer to libraries
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/libs )

if(LOWFAT_BUILD)
    add_definitions ( -DCOFFEE_LOWFAT )
endif()

include ( BuildOptions )
include ( PrecompiledHeader )
if(BUILD_COVERAGE)
    include ( CodeCoverage )
    APPEND_COVERAGE_COMPILER_FLAGS()
endif()

if(COFFEE_GENERATE_LIBRARIES)
    include ( BuildFunctions )

    include ( BuildFlags )
endif()

# Primary includes
include_directories (
    ${CMAKE_SOURCE_DIR}/internal/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    )

# General include directories, not ours
include_directories ( SYSTEM
    ${CMAKE_BINARY_DIR}/libs/include
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

if(COFFEE_GENERATE_LIBRARIES)
    # Add subdir for core modules
    add_subdirectory(coffee)

    # Add subdir for generation of language bindings
    add_subdirectory(bindings)

    if(GENERATE_PROGRAMS)
        if(COFFEE_GENERATE_HTML)
            set ( CMAKE_EXECUTABLE_SUFFIX ".html" )
        endif()
        # Tools, includes the GUI profiler
        add_subdirectory(tools)

        # Add subdir for example applications
        if(GENERATE_EXAMPLES)
            add_subdirectory(examples)
        endif()
        if(GENERATE_TESTS)
            add_subdirectory(examples/unit-tests)
        endif()
    endif()

    # Add subdir for extensions, weak bindings of libraries
    add_subdirectory(ext)

    install ( FILES
        ${CMAKE_SOURCE_DIR}/cmake/Find/FindCoffee.cmake

        DESTINATION
        share/cmake/Modules)
endif()

# Add documentation directory, not built by default (increases build time)
add_subdirectory(docs-gen)

if(COFFEE_GENERATE_LIBRARIES)
    install ( DIRECTORY
        ${CMAKE_SOURCE_DIR}/include
        DESTINATION
        .
        )
endif()

# Add automatic linking directory, useful to get builds up quickly
link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
    )
