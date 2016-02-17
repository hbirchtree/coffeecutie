project( Coffee C CXX )
cmake_minimum_required( VERSION 3.1 )

# We set some general options for building
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

set ( CMAKE_BUILD_TYPE Debug )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules )

# Make it verbose
set ( CMAKE_VERBOSE_MAKEFILE on )

# Fiddle with warning flags
if (NOT WIN32)
    # These are effectively used by Clang and GCC, descriptive names
    add_definitions (
#        -Wall
#        -Werror
#        -Wpadded
        )
else()
    # For Windows, we disable most annoying warnings to save compilation time.
    # These include system headers (which are constantly giving off warnings)
    #  as well as small warnings for padding and etc.
    # Microsoft makes the worst headers of them all.
    add_definitions (
        -W4
        )
endif()

option ( COFFEE_DOCS "Generate documentation" OFF )

# Language bindings
option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG bindings" OFF )

# D language bindings, builds a module
option ( COFFEE_DLANG "Build D-language binding" OFF)
# Python bindings
option ( COFFEE_PYTHON "Build Python binding" OFF)
# CSharp bindings
option ( COFFEE_CSHARP "Build C# binding" OFF)
###

# Builds core,audio,media,sdl2 and etc. as static libraries which are recommended for ease of use.
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

# Whether we should opt for the GLES renderer or not. This will switch the GL implementation as well, making OpenGL 3.3, 4.3 and 4.5 unavailable.
option ( COFFEE_BUILD_GLES "Build with GLES 3.0 renderer instead of desktop GL")

# External libraries
# Whether we should build FFMPEG-dependent modules, notably media
option ( COFFEE_BUILD_FFMEDIA "Build with FFMPEG library" ON)
# Whether we should build the legacy GLBinding module for OpenGL abstraction.
option ( COFFEE_BUILD_GLBINDING "Build with GLBinding library" ON)
# Whether we should build with Assimp support
option ( COFFEE_BUILD_ASSIMP "Build with Assimp loader" ON)
# Whether we should build the OpenAL-dependent audio module
option ( COFFEE_BUILD_OPENAL "Build with OpenAL" ON)
# Whether we should build with OpenVR support
option ( COFFEE_BUILD_OPENVR "Build with OpenVR" ON)
# Whether we should build with support for the PCL extension
option ( COFFEE_BUILD_PCL "Build with PCL" OFF)
# Whether we should build with support for the Kinect extension
option ( COFFEE_BUILD_NECT "Build with Kinect support" OFF)
# Whether we should build with support for the Qt extension
option ( COFFEE_BUILD_QT "Build with Qt support" ON)
###

# This is used inside modules to refer to libraries
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

# Build time strings, embedded within constexpr strings to keep track of when a build was made.
# Because file timestamps are unreliable.
string (TIMESTAMP CBUILDTIME "%y.%m.%d.%H%M")
add_definitions(-D__CBUILDTIME__="${CBUILDTIME}")

# The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
# Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID)
        #Android's compiler doesn't support target_compile_features :(
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()
endmacro()

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES)
    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}")
    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_enable_cxx11(${TARGET})

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        lib
        )

endmacro()

macro(COFFEE_ADD_LIBRARY TARGET SOURCES)
    # Just a little simplification
    coffee_add_elibrary(${TARGET} ${COFFEE_LINK_OPT} "${SOURCES}")
endmacro()

#Android only uses shared libraries which are loaded, all else uses typical executables
macro(COFFEE_ADD_EXAMPLE TARGET SOURCES)
    if(ANDROID)
        add_library(${TARGET} SHARED ${SOURCES})
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    else()
        add_executable(${TARGET} ${SOURCES})
    endif()
    target_enable_cxx11(${TARGET})

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        bin
        )
endmacro()

# General include directories
include_directories ( SYSTEM
    ${CMAKE_BINARY_DIR}/libs/include
    libs
    )

include_directories (
    ${CMAKE_SOURCE_DIR}
    include
    )

# Add automatic linking directory, useful to get builds up quickly
link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
    )

# Static builds
if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

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

# Add subdir for extensions, weak bindings of libraries
add_subdirectory(ext)
