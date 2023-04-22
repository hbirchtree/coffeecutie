#
# Here lies examples of code usage
#
#

#
# Core
#

add_subdirectory (core/env-test)

if(NOT LOWFAT_BUILD)
    add_subdirectory (components)
    add_subdirectory (gleam/comp_app)
endif()

# ASIO examples, we leave this as core
if(BUILD_ASIO)
#    add_subdirectory(asio/base-client)
#    add_subdirectory(asio/base-server)
#    add_subdirectory(asio/irc-client)
endif()

#
#

#
# GLeam
#

if(BUILD_GLEAM)
    add_subdirectory(gleam/rhi/base-test)
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
# Font rendering, stb_truetype.h
#

if(BUILD_DISCORD_RPC)
    add_subdirectory(discord/basic)
endif()
