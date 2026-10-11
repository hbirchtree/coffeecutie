coffee_application(
  TARGET BlamGCGateway
  TITLE "GameCube to GNS gateway"
  SOURCES main.cpp
  LIBRARIES Core
)

target_include_directories(BlamGCGateway PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(BlamGCGateway PRIVATE
    GameNetworkingSockets::GameNetworkingSockets_s
)
