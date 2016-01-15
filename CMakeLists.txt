project( CoffeeCore C CXX )
cmake_minimum_required( VERSION 3.1 )

#We set some general options for building
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

#Deprecated, used to make SWIG C# binding, doesn't work so well
option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG CSharp binding" ON )
#Builds core,audio,media,sdl2 and etc. as static libraries which are recommended for ease of use.
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

#Whether we should opt for the GLES renderer or not. This will switch the GL implementation as well, making OpenGL 3.3, 4.3 and 4.5 unavailable.
option ( COFFEE_BUILD_GLES "Build with GLES 3.0 renderer instead of desktop GL")

#Whether we should build FFMPEG-dependent modules, notably media
option ( COFFEE_BUILD_FFMEDIA "Build with FFMPEG library" ON)
#Whether we should build the legacy GLBinding module for OpenGL abstraction.
option ( COFFEE_BUILD_GLBINDING "Build with GLBinding library" ON)

option ( COFFEE_BUILD_ASSIMP "Build with Assimp loader" ON)
#Whether we should build the OpenAL-dependent audio module
option ( COFFEE_BUILD_OPENAL "Build with OpenAL" ON)
#Whether we should build with OpenVR support
option ( COFFEE_BUILD_OPENVR "Build with OpenVR" ON)
#Whether we should build with support for the PCL extension
option ( COFFEE_BUILD_PCL "Build with PCL" OFF)
#Whether we should build with support for the Kinect extension
option ( COFFEE_BUILD_NECT "Build with Kinect support" OFF)
#Whether we should build with support for the Qt extension
option ( COFFEE_BUILD_QT "Build with Qt support" ON)

#This is used inside modules to refer to libraries
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

#Build time strings, embedded within constexpr strings to keep track of when a build was made.
#Because file timestamps are unreliable.
string (TIMESTAMP CBUILDTIME "%y.%m.%d.%H%M")
add_definitions(-D__CBUILDTIME__="${CBUILDTIME}")
#The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
#Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID)
        add_definitions("-std=c++11")
    else()
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()
endmacro()

#Fiddle with warning flags
if (NOT WIN32)
    #These are effectively used by Clang and GCC, descriptive names
    add_definitions (
#        -Wall
#        -Werror
#        -Wpadded
        )
else()
    #For Windows, we disable most annoying warnings to save compilation time.
    #These include system headers (which are constantly giving off warnings)
    #  as well as small warnings for padding and etc.
    add_definitions (
        -W4
        )
endif()

#General include directories
include_directories ( SYSTEM
    ${CMAKE_BINARY_DIR}/libs/include
    libs
    )

include_directories (
    ${CMAKE_SOURCE_DIR}
    include
    coffee
    )

#Add automatic linking directory, useful to get builds up quickly
link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
    )

#Static builds
if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

#Core include directories; I'm putting them here temporarily
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

#Add subdir for core modules
add_subdirectory(coffee)

#Add subdir for example applications
add_subdirectory(examples)
#Add documentation directory, not built by default (increases build time)
add_subdirectory(docs)

#SWIG build, not functional anymore
if(COFFEE_BUILD_SWIG_BINDING_)
    add_subdirectory(swig)
endif()

#Extension include directories
set (
    COFFEE_EXT_PCL_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/ext/pcl-shim/include
    )
set (
    COFFEE_EXT_FREENECT2_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/ext/kinect-shim/include
    ${COFFEE_CORE_INCLUDE_DIR}
    )

#Add subdir for extensions, weak bindings of libraries
add_subdirectory(ext)
