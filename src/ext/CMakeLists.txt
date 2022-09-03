# Discord Rich Presence API
option ( BUILD_DISCORD_RPC "Build with Discord Rich Presence" OFF )

if(BUILD_DISCORD_RPC)
    add_subdirectory(discord)
endif()
