 set ( GIT_DEP_BUILDVARIANT "steam.amd64" CACHE STRING "" )
 set ( CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_SOURCE_DIR}/toolchain/cmake/Toolchains/cxx11.toolchain.cmake" CACHE STRING "" )
 set ( BUILD_CPP11 "ON" CACHE STRING "" )
 set ( BUILD_ASSIMP "ON" CACHE STRING "" )
 set ( CMAKE_BUILD_TYPE "$ENV{CONFIGURATION}" CACHE STRING "" )
 set ( CMAKE_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_DIR}/steam.amd64" CACHE STRING "" )
 set ( BUILD_BINARIES "$ENV{GENERATE_PROGRAMS}" CACHE STRING "" )
 set ( SKIP_GRAPHIC_TESTS "$ENV{HEADLESS}" CACHE STRING "" )
 set ( BUILD_ASSIMP "ON" CACHE STRING "" )
