#
# Find SDL2_android_main.c
#

set ( SDL2_ROOT_DIR CACHE PATH "Path to SDL2 source directory" )

find_file ( SDL2_ANDROID_MAIN_FILE_TMP
    SDL_android_main.c

    PATHS
    ${CMAKE_BINARY_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    $ENV{SDL2_ROOT_DIR}/src/main/android

    PATH_SUFFIXES
    src
    )

if(SDL2_ANDROID_MAIN_FILE_TMP)
    set ( SDL2_ANDROID_MAIN_FILE
        ${SDL2_ANDROID_MAIN_FILE_TMP}
        CACHE FILEPATH
        "Path to SDL_android_main.c for JNI functions"
        )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2main
    REQUIRED_VARS SDL2_ANDROID_MAIN_FILE
    )
