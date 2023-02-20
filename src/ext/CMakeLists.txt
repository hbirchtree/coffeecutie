# Discord Rich Presence API
option ( BUILD_DISCORD_RPC "Build with Discord Rich Presence" OFF )
option ( BUILD_IMGUI "Build with ImGui" ON )

if(BUILD_DISCORD_RPC)
    add_subdirectory(discord)
endif()

if(BUILD_IMGUI)
    add_subdirectory(imgui)
endif()
