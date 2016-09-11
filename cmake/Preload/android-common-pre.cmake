set ( CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/out CACHE PATH "" )
set ( NATIVE_LIBRARY_DIR "${NATIVE_LIBRARY_ROOT}/Android" CACHE PATH "" )

set ( ANDROID_STL gnustl_static CACHE STRING "" )

include_directories ( "${NATIVE_LIBRARY_DIR}/include" )

set ( SDL2_ANDROID_MAIN_FILE
    "${NATIVE_LIBRARY_DIR}/src/SDL_android_main.c"
    CACHE FILEPATH "" )

set ( SDL2_INCLUDE_DIR "${NATIVE_LIBRARY_DIR}/include/SDL2" CACHE PATH "" )
set ( OPENAL_INCLUDE_DIR "${NATIVE_LIBRARY_DIR}/include/AL" CACHE PATH "" )

# Required as of now
set ( COFFEE_BUILD_GLES ON CACHE BOOL "" )

# Does not work with Android as of yet
set ( COFFEE_BUILD_OPENSSL OFF  CACHE BOOL "" )
set ( COFFEE_BUILD_QT OFF  CACHE BOOL "" )
set ( COFFEE_BUILD_NECT OFF CACHE BOOL "" )
set ( COFFEE_BUILD_PCL OFF CACHE BOOL "" )
