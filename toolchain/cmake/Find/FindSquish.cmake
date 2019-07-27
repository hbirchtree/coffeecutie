set ( SQUISH_PATHS
    /usr
    /usr/local
    /Library/Frameworks
    ~/Library/Frameworks
    ${SQUISH_DIR}
    )

find_path ( SQUISH_INCLUDE_DIR_TMP
    NAMES
    squish.h

    PATHS ${SQUISH_PATHS}

    PATH_SUFFIXES
    include
    )

find_library ( SQUISH_LIBRARY_TMP
    NAMES squish

    PATHS ${SQUISH_PATHS}

    PATH_SUFFIXES
    lib # Default stuff
    "lib/${CMAKE_LIBRARY_ARCHITECTURE}" # CMake architecture path
    "lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release"
    "lib/${ANDROID_ABI}" "${ANDROID_ABI}" # Android paths
    "lib/${WINDOWS_ABI}"

    "lib/${CMAKE_LIBRARY_ARCHITECTURE_SDL}" # CMake architecture path
    "lib/${CMAKE_LIBRARY_ARCHITECTURE_SDL}/Release"
    )

if(NOT TARGET Squish)
    add_library( Squish STATIC IMPORTED )
endif()

set_target_properties ( Squish PROPERTIES
    IMPORTED_LOCATION "${SQUISH_LIBRARY_TMP}"
    INTERFACE_INCLUDE_DIRECTORIES "${SQUISH_INCLUDE_DIR_TMP}"
    )

if(SQUISH_LIBRARY_TMP AND SQUISH_INCLUDE_DIR_TMP)
    set ( SQUISH_FOUND TRUE )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Squish
    REQUIRED_VARS
    SQUISH_INCLUDE_DIR_TMP
    SQUISH_LIBRARY_TMP
    SQUISH_FOUND
    )
