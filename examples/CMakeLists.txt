#
# Here lies examples of code usage
#
#

if(COFFEE_BUILD_OCULUSVR)
    message ( "-- Examples built with Oculus support" )
else()
    message ( "-- Examples built without Oculus support" )
endif()

if(NOT COFFEE_BUILD_OPENAL)
    message ( "-- Examples built without OpenAL support" )
endif()

#
# Core
#

add_subdirectory (core/env-test)

if(COFFEE_BUILD_OPENSSL)
    # ASIO examples, we leave this as core
    add_subdirectory(asio/base-client)
    #add_subdirectory(asio/base-server)
    add_subdirectory(asio/irc-client)
endif()

# Not going to say.
add_subdirectory(rsread)

# Casual fun
add_subdirectory (experiments/cgb-test)

#
#

#
# SDL2
#

if(COFFEE_BUILD_SDL2)
    # Sprite rendering with SDL2
    add_subdirectory(sdl_sprite/base-test)
    # Joystick visualizer
    add_subdirectory(input/joystick-test)
endif()

#
#

#
# OpenAL
#

# Examples of CAudio API through OpenAL and Coffee audio abstraction layer
if(COFFEE_BUILD_OPENAL)
    add_subdirectory (audio/caudio-test)
endif()

#
#

#
# GLeam
#

if(COFFEE_BUILD_SDL2 AND COFFEE_BUILD_GLEAM)
    if(COFFEE_BUILD_GLEAM_RHI)
        # Supports GL 3.3+ and GL ES 3.0+ APIs
        add_subdirectory(gleam/rhi/base-test)
        if(COFFEE_BUILD_OPENVR)
            add_subdirectory(gleam/rhi/vr-test)
        endif()
    endif()

    if(NOT COFFEE_BUILD_GLES)
        add_subdirectory(gleam/base-test)
        add_subdirectory(gleam/raytrace-test)
    endif()

    add_subdirectory(gleam/shadow-test)
endif()

#
#

#
# CBlam
#

# Examples of the CBlam API and its import of Blam engine data
if(COFFEE_BUILD_CBLAM)
    add_subdirectory (blam/cblam-testing)
endif()

#
#

#
# FFMedia
#

# Examples of media playback and capture through CFFMedia, powered by FFMPEG
if(COFFEE_BUILD_SDL2 AND COFFEE_BUILD_FFMEDIA AND COFFEE_BUILD_GLBINDING)
    add_subdirectory (media/media-test)
endif()

#
#

#
# PCL/Kinect
#

# Manipulation of point cloud data through PCL
if(COFFEE_BUILD_PCL AND COFFEE_BUILD_NECT)
    add_subdirectory (pcl/pcl-test)
endif()

#
#

#
# Qt
#

# Qt widgets and etc.
if(COFFEE_BUILD_QT)
    add_subdirectory(qt/node-editor-test)
endif()

#
#

#
# VR
#

if(COFFEE_BUILD_OPENVR) # AND COFFEE_BUILD_OCULUSVR
    add_subdirectory(vr/base-test)
#    add_subdirectory(vr/hellovr)
#    add_subdirectory(vr/hellovr-rhi)
endif()

#
#


#
# Chromium Embedded Framework
#

# Offscreen rendering test
if(COFFEE_BUILD_CEF)
    add_subdirectory(cef/base-test)
endif()

#
#

#
# GLBinding APIs (deprecated)
#


if(COFFEE_BUILD_GLBINDING)
    if(COFFEE_BUILD_SDL2)
        # Library loading and interface implementation
        add_subdirectory (core/dynamic-test)

        # Examples of graphics rendering through OpenGL with several different techniques in use
        if(COFFEE_BUILD_ASSIMP)
            add_subdirectory (opengl/ctest)
        endif()
        add_subdirectory (opengl/ctest-hud)
        add_subdirectory (opengl/ctest-async-pbo)
        add_subdirectory (opengl/ctest-shaderswitch)
    endif()
endif()

#
#

#
# SWIG APIs
#

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(wrapper/native-test)
endif()

#
#

#
# Unit tests
#

if(COFFEE_TESTS)
    add_subdirectory(unit-tests)
endif()

#
#

#
# Font rendering, stb_truetype.h
#

add_subdirectory(font-rendering)

#
#
