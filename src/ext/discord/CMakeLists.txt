find_package(discord-rpc REQUIRED)

coffee_library(
  TARGET DiscordLatte
  SOURCES private/discord_binding.cpp
  LIBRARIES Core OnlineService ASIO DiscordRPC::discord-rpc
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
