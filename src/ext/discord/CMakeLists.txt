set ( BUILD_EXAMPLES OFF )
set ( ENABLE_IO_THREAD OFF )

dependency_get (
    SOURCE hbirchtree/native-library-bundle
    TAG ${NATIVE_LIB_BUNDLE_TAG}
    REQUIRED
    NAMES
        discord-rpc=DiscordRpc
    )

find_package ( DiscordRpc REQUIRED )

coffee_library (
    TARGET DiscordLatte

    SOURCES private/discord_binding.cpp
    LIBRARIES Core OnlineService ASIO DiscordRPC::discord-rpc
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

dependency_link (
    TARGET DiscordLatte
    DEPENDENCIES
        discord-rpc=DiscordRpc
    )
