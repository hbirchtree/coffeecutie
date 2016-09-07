set ( CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/out CACHE PATH "" )
set ( NATIVE_LIBRARY_DIR "${NATIVE_LIBRARY_ROOT}/Android" CACHE PATH "" )

# Android toolchain-specific
set ( ANDROID_STL gnustl_static CACHE STRING "" )
set ( ANDROID_NATIVE_API_LEVEL 12 CACHE STRING "" )
set ( ANDROID_ABI "armeabi" CACHE STRING "" )

set ( COFFEE_BUILD_GLEAM_RHI OFF CACHE BOOL "" )

string ( REGEX REPLACE "([a-zA-Z0-9_-]+)\ .*" "\\1" ANDROID_LIB_SUFFIX "${ANDROID_ABI}"  )

include_directories ( "${NATIVE_LIBRARY_DIR}/include" )
link_directories ( "${NATIVE_LIBRARY_DIR}/${ANDROID_LIB_SUFFIX}" )

set ( SDL2_ANDROID_MAIN_FILE
    "${NATIVE_LIBRARY_DIR}/src/SDL_android_main.c"
    CACHE FILEPATH "" )

set ( SDL2_INCLUDE_DIR "${NATIVE_LIBRARY_DIR}/include/SDL2" CACHE PATH "" )
set ( OPENAL_INCLUDE_DIR "${NATIVE_LIBRARY_DIR}/include/AL" CACHE PATH "" )
