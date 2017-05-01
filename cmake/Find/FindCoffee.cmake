
#
# Autogenerated by tools/scripts/gen-findscript.sh
# Changes made to this file will NOT be saved
# This find-script will make it easier to add several libraries for external use 
#

set ( COFFEE_SEARCH_PATHS
    /usr/local
    /usr
    /opt

    ~/Library/Frameworks
    /Library/Frameworks

    "${COFFEE_ROOT_DIR}"
    "$ENV{COFFEE_ROOT_DIR}"
    )
set ( COFFEE_LIBRARY_SUFFIXES
    lib
    lib64
    lib/${ANDROID_ABI}
    )

find_path ( COFFEE_INCLUDE_DIR_TMP
    coffee/core/coffee.h
    coffee/CCore

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    include
    )

if(COFFEE_INCLUDE_DIR_TMP)
    set ( COFFEE_INCLUDE_DIR "${COFFEE_INCLUDE_DIR_TMP}" CACHE PATH "" )
endif()

find_library ( COFFEE_CORE_LIBRARY_TMP
    CoffeeCore

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_CORE_LIBRARY_TMP)
    set ( COFFEE_CORE_LIBRARY "${COFFEE_CORE_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_GLEAM_LIBRARY_TMP
    CoffeeGLeam

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_GLEAM_LIBRARY_TMP)
    set ( COFFEE_GLEAM_LIBRARY "${COFFEE_GLEAM_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_ASIO_LIBRARY_TMP
    CoffeeASIO

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_ASIO_LIBRARY_TMP)
    set ( COFFEE_ASIO_LIBRARY "${COFFEE_ASIO_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_OPENAL_LIBRARY_TMP
    CoffeeOpenAL

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_OPENAL_LIBRARY_TMP)
    set ( COFFEE_OPENAL_LIBRARY "${COFFEE_OPENAL_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_SDL2_LIBRARY_TMP
    CoffeeSDL2

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_SDL2_LIBRARY_TMP)
    set ( COFFEE_SDL2_LIBRARY "${COFFEE_SDL2_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_BLAM_LIBRARY_TMP
    CoffeeBlam

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_BLAM_LIBRARY_TMP)
    set ( COFFEE_BLAM_LIBRARY "${COFFEE_BLAM_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_GRAPHICS_LIBRARY_TMP
    CoffeeGraphicsAPI

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_GRAPHICS_LIBRARY_TMP)
    set ( COFFEE_GRAPHICS_LIBRARY "${COFFEE_GRAPHICS_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_IMAGE_LIBRARY_TMP
    CoffeeImage

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_IMAGE_LIBRARY_TMP)
    set ( COFFEE_IMAGE_LIBRARY "${COFFEE_IMAGE_LIBRARY_TMP}" CACHE STRING "" )
endif()

find_library ( COFFEE_ASSIMP_LIBRARY_TMP
    CoffeeAssimp

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    ${COFFEE_LIBRARY_SUFFIXES}
    )

if(COFFEE_ASSIMP_LIBRARY_TMP)
    set ( COFFEE_ASSIMP_LIBRARY "${COFFEE_ASSIMP_LIBRARY_TMP}" CACHE STRING "" )
endif()

if(ANDROID)
    find_library ( COFFEE_ANDROID_LIBRARY_TMP
        AndroidCore

        PATHS
        ${COFFEE_SEARCH_PATHS}

        PATH_SUFFIXES
        ${COFFEE_LIBRARY_SUFFIXES}
        )
    if(COFFEE_ANDROID_LIBRARY_TMP)
        set ( COFFEE_ANDROID_LIBRARY "${COFFEE_ANDROID_LIBRARY_TMP}" CACHE STRING "" )
    endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args (
    Coffee

    REQUIRED_VARS
    COFFEE_INCLUDE_DIR
    COFFEE_CORE_LIBRARY
    COFFEE_GLEAM_LIBRARY
    COFFEE_ASIO_LIBRARY
    COFFEE_OPENAL_LIBRARY
    COFFEE_SDL2_LIBRARY
    COFFEE_BLAM_LIBRARY
    COFFEE_GRAPHICS_LIBRARY
    COFFEE_IMAGE_LIBRARY
    )
