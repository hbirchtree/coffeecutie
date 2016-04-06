#
# Find SDL2_android_main.c
#

set ( SDL2_ANDROID_MAIN_FILE CACHE FILEPATH "Path to SDL_android_main.c for JNI functions" )

find_file ( SDL2_ANDROID_MAIN_FILE_TMP
    SDL_android_main.c
    PATHS
    ${CMAKE_BINARY_DIR}/libs/src
    PATH_SUFFIXES
    src
    )

if(SDL2_ANDROID_MAIN_FILE_TMP)
    set ( SDL2_ANDROID_MAIN_FILE ${SDL2_ANDROID_MAIN_FILE_TMP} CACHE FILEPATH "Path to SDL_android_main.c for JNI functions" )
endif()

mark_as_advanced(SDL2_ANDROID_MAIN_FILE)
