set ( OPENAL_SEARCH_PATHS
    /usr
    /usr/local
    ${OPENAL_ROOT}
    ${CMAKE_BINARY_DIR}/libs
    "C:/Program Files/OpenAL 1.1 SDK"
    "C:/Program Files (x86)/OpenAL 1.1 SDK"
    ~/Library/Frameworks
    /Library/Frameworks
    ${NATIVE_LIBRARY_DIR}
    )

if(ANDROID)
    list ( APPEND OPENAL_SEARCH_PATHS
        /home/coffee/libs
        )
endif()

find_path ( OPENAL_INCLUDE_DIR
    al.h
    alc.h

    PATH_SUFFIXES
    include
    include/AL

    PATHS
    ${OPENAL_SEARCH_PATHS}

    NO_CMAKE_FIND_ROOT_PATH
    )

find_path ( OPENAL_INCLUDE_DIR
    al.h
    alc.h

    PATH_SUFFIXES
    include
    include/AL

    PATHS
    ${OPENAL_SEARCH_PATHS}
    )

find_library ( OPENAL_LIBRARY_TMP
    NAMES
    openal
    OpenAL
    OpenAL32

    PATH_SUFFIXES
    lib
    "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
    "lib/${ANDROID_ABI}"
    "${ANDROID_ABI}"
    libs/Win64
    64/link

    PATHS
    ${OPENAL_SEARCH_PATHS}

    NO_CMAKE_FIND_ROOT_PATH
    )

find_library ( OPENAL_LIBRARY_TMP
    NAMES
    openal
    OpenAL
    OpenAL32
    )

find_library ( OPENAL_EFX_LIBRARY_TMP
    NAMES
    EFX-Util

    PATH_SUFFIXES
    lib
    lib/${ANDROID_ABI}
    libs/Win64/EFX-Util_MT
    64/link

    PATHS
    ${OPENAL_SEARCH_PATHS}

    NO_CMAKE_FIND_ROOT_PATH
    )

find_library ( OPENAL_EFX_LIBRARY_TMP
    NAMES
    EFX-Util
    )

if(OPENAL_INCLUDE_DIR)
    set ( OPENAL_INCLUDE_DIR "${OPENAL_INCLUDE_DIR}" CACHE PATH "OpenAL include directory" )
endif()
if(OPENAL_LIBRARY_TMP)
    set ( OPENAL_LIBRARY "${OPENAL_LIBRARY_TMP}" CACHE FILEPATH "OpenAL library file" )
endif()
if(OPENAL_EFX_LIBRARY_TMP)
    set ( OPENAL_EFX_LIBRARY "${OPENAL_EFX_LIBRARY_TMP}" CACHE FILEPATH "OpenAL EFX library file" )
endif()

#
# NOTE: Linking to this library on OSX and iOS is bad.
# It will attempt to link (statically/dynamically) into a directory.
# Don't try to link this on OSX and iOS.
#
if(NOT TARGET OpenAL)
    add_library ( OpenAL SHARED IMPORTED )

    set_target_properties ( OpenAL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${OPENAL_LIBRARY}"
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
    )

    add_library ( OpenAL_EFX SHARED IMPORTED )

    set_target_properties ( OpenAL_EFX PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${OPENAL_EFX_LIBRARY}"
        IMPORTED_LOCATION "${OPENAL_EFX_LIBRARY}"
    )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenAL REQUIRED_VARS OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
