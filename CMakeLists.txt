project( CoffeeCore C CXX )
cmake_minimum_required( VERSION 3.1 )

option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG CSharp binding" ON )
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

option ( COFFEE_BUILD_GLES "Build with GLES 3.0 renderer instead of desktop GL")

#TODO: Make these options functional
option ( COFFEE_BUILD_FFMEDIA "Build with FFMPEG library" ON)
option ( COFFEE_BUILD_GLBINDING "Build with GLBinding library" ON)
option ( COFFEE_BUILD_OPENAL "Build with OpenAL" ON)
option ( COFFEE_BUILD_PCL "Build with PCL" OFF)
option ( COFFEE_BUILD_NECT "Build with Kinect support" OFF)
option ( COFFEE_BUILD_QT "Build with Qt support" ON)

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

#Build time strings
string (TIMESTAMP CBUILDTIME "%y.%m.%d.%H%M")
add_definitions(-D__CBUILDTIME__="${CBUILDTIME}")
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

#Warning flags
if (NOT WIN32)
    add_definitions (
        -Wall
#        -Wpadded
    )
else()
    add_definitions ( -W4 )
endif()

#Includes
include_directories (
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/libs/include
        include
        coffee
    )

include_directories ( SYSTEM
        libs
    )

#Mark warnings as errors (GCC)
if(CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

#Add automatic linking directory
link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
    )

#Static builds
if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

add_custom_target(DummyPlug)

if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    include_directories("${LIBUNWIND_INCLUDE_DIR}")
endif()

#Include directories; I'm putting them here temporarily
set (
    COFFEE_CORE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/core/include
        ${CMAKE_SOURCE_DIR}/coffee/core
    )
set (
    COFFEE_ASSIMP_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/assimp/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_AUDIO_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/audio/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_IMAGE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/image/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_OPENVR_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/openvr/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_SDL2_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/sdl2/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_GRAPHICS_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )
set (
    COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_SDL2_INCLUDE_DIR}
    )
set (
    COFFEE_BLAM_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/blam/include
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    )
set (
    COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/gleam/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_SDL2_INCLUDE_DIR}
    )

#Extensions
set (
    COFFEE_EXT_FREENECT2_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/ext/kinect-shim/include
        ${COFFEE_CORE_INCLUDE_DIR}
    )

add_subdirectory(coffee)

add_subdirectory(examples)
add_subdirectory(docs)

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(swig)
endif()

#add_subdirectory(libs)

#PCL extension

set (
    COFFEE_EXT_PCL_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/ext/pcl-shim/include
    )

add_subdirectory(ext)
