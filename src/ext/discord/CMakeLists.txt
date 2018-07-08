set ( BUILD_EXAMPLES OFF )

add_subdirectory ( discord-rpc )

coffee_library (
    TARGET DiscordLatte
    SOURCES private/discord_binding.cpp
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_link_libraries ( DiscordLatte
    PRIVATE
    discord-rpc
    )
