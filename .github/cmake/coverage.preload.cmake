set( ENV{SOURCE_DIR} ${CMAKE_SOURCE_DIR} )
message( "Source dir: $ENV{SOURCE_DIR}" )
set ( GIT_DEP_BUILDVARIANT "ubuntu.amd64" CACHE STRING "" )
set ( CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_SOURCE_DIR}/toolchain/cmake/Toolchains/linux-generic_linux.toolchain.cmake" CACHE STRING "" )
set ( BUILD_COVERAGE "ON" CACHE STRING "" )
set ( BUILD_ASAN "ON" CACHE STRING "" )
set ( BUILD_EXAMPLES "OFF" CACHE STRING "" )
set ( SKIP_HIGHMEM_TESTS "ON" CACHE STRING "" )
set ( SKIP_LINKAGE_TEST "ON" CACHE STRING "" )
set ( SKIP_OPENVR_TEST "ON" CACHE STRING "" )
set ( CMAKE_BUILD_TYPE "Debug" CACHE STRING "" )
set ( CMAKE_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_DIR}/coverage" CACHE STRING "" )
set ( BUILD_BINARIES "$ENV{GENERATE_PROGRAMS}" CACHE STRING "" )
set ( SKIP_GRAPHIC_TESTS "$ENV{HEADLESS}" CACHE STRING "" )
set ( BUILD_ASSIMP "ON" CACHE STRING "" )
