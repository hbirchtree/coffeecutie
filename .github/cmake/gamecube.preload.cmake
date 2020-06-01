set( ENV{SOURCE_DIR} ${CMAKE_SOURCE_DIR} )
message( "Source dir: $ENV{SOURCE_DIR}" )
set ( GIT_DEP_BUILDVARIANT "gamecube" CACHE STRING "" )
set ( CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_SOURCE_DIR}/toolchain/cmake/Toolchains/none-cube-powerpc-eabi_linux.toolchain.cmake" CACHE STRING "" )
set ( BUILD_GLEAM "OFF" CACHE STRING "" )
set ( CMAKE_BUILD_TYPE "$ENV{CONFIGURATION}" CACHE STRING "" )
set ( CMAKE_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_DIR}/gamecube" CACHE STRING "" )
set ( BUILD_BINARIES "$ENV{GENERATE_PROGRAMS}" CACHE STRING "" )
set ( SKIP_GRAPHIC_TESTS "$ENV{HEADLESS}" CACHE STRING "" )
set ( BUILD_LIBOGC "ON" CACHE STRING "" )
set ( BUILD_ASIO "OFF" CACHE STRING "" )
set ( BUILD_AUDIO "OFF" CACHE STRING "" )
set ( BUILD_SDL2 "OFF" CACHE STRING "" )
set ( BUILD_OPENGL "OFF" CACHE STRING "" )
set ( BUILD_GRAPHICS_COMMON "OFF" CACHE STRING "" )
set ( BUILD_BLAM "OFF" CACHE STRING "" )
set ( BUILD_GX "ON" CACHE STRING "" )
