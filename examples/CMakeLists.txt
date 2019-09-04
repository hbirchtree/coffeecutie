#
# Here lies examples of code usage
#
#

#
# Core
#

add_subdirectory (core/env-test)
add_subdirectory (core/power-test)
add_subdirectory (core/gpu-query)

if(NOT LOWFAT_BUILD)
    add_subdirectory (components)
    add_subdirectory (gleam/comp_app)
endif()

# ASIO examples, we leave this as core
if(BUILD_ASIO)
    add_subdirectory(asio/base-client)
#    add_subdirectory(asio/base-server)
    add_subdirectory(asio/irc-client)
endif()

# Not going to say.
#add_subdirectory(rsread)

#
#

#
# SDL2
#

if(BUILD_SDL2)
    # Sprite rendering with SDL2
    add_subdirectory(sdl_sprite/base-test)
    # Joystick visualizer
    if(NOT APPLE)
        add_subdirectory(input/joystick-test)
    endif()
endif()

#
#

#
# OpenAL
#

# Examples of CAudio API through OpenAL and Coffee audio abstraction layer
if(BUILD_AUDIO AND BUILD_OPENAL AND BUILD_GLEAM)
    if(NOT APPLE)
        add_subdirectory (audio/caudio-test)
    endif()
endif()

#
#

#
# GLeam
#

if(BUILD_GLEAM)
    add_subdirectory(gleam/rhi/base-test)
    add_subdirectory(gleam/compress-formats)
    if(BUILD_OPENGL)
        add_subdirectory(gleam/shadow-test)
    endif()
endif()

#
#

#
# CBlam
#

# Examples of the CBlam API and its import of Blam engine data
if(BUILD_BLAM AND BUILD_GLEAM)
    add_subdirectory (blam/cblam-testing)
endif()

#
#

#
# Assimp
#

if(BUILD_ASSIMP)
    add_subdirectory(assimp/model-load)
endif()

#
#

#
# Chromium Embedded Framework
#

# Offscreen rendering test
if(BUILD_CEF)
    add_subdirectory(cef/base-test)
endif()

#
#

#
# SWIG APIs
#

if(BUILD_SWIG_BINDING)
    add_subdirectory(wrapper/native-test)
endif()

#
#

#
# Font rendering, stb_truetype.h
#

add_subdirectory(font-rendering)

if(BUILD_DISCORD_RPC)
    add_subdirectory(discord/basic)
endif()
