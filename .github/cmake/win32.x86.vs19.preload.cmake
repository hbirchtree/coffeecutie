set( ENV{SOURCE_DIR} ${CMAKE_SOURCE_DIR} )
message( "Source dir: $ENV{SOURCE_DIR}" )
set ( GIT_DEP_BUILDVARIANT "win32.x86.vs19" CACHE STRING "" )
set ( CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_SOURCE_DIR}/toolchain/cmake/Toolchains/windows-win32_windows.toolchain.cmake" CACHE STRING "" )
set ( CMAKE_BUILD_TYPE "$ENV{CONFIGURATION}" CACHE STRING "" )
set ( CMAKE_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_DIR}/win32.x86.vs19" CACHE STRING "" )
set ( BUILD_BINARIES "$ENV{GENERATE_PROGRAMS}" CACHE STRING "" )
set ( SKIP_GRAPHIC_TESTS "$ENV{HEADLESS}" CACHE STRING "" )
