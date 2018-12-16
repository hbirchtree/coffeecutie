set ( BUILD_EXAMPLES OFF )
set ( ENABLE_IO_THREAD OFF )

add_subdirectory ( discord-rpc )

coffee_library (
    TARGET DiscordLatte
    NO_EXPORT

    SOURCES private/discord_binding.cpp
    LIBRARIES Core OnlineService ASIO
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_link_libraries ( DiscordLatte PRIVATE
    discord-rpc
    )

target_include_directories ( DiscordLatte PRIVATE
    discord-rpc/include
    )
