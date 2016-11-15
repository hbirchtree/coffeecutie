#
# Coffee will only accept a single include directory for all libraries for now.
# Anything more is effort.
#

set ( COFFEE_INCLUDE_DIR    "" CACHE PATH "Coffee include directory" )

set ( COFFEE_CORE_LIBRARY   "" CACHE FILEPATH "Coffee Core library" )
set ( COFFEE_OCULUS_LIBRARY "" CACHE FILEPATH "Coffee OculusVR library" )
set ( COFFEE_OPENVR_LIBRARY "" CACHE FILEPATH "Coffee OpenVR library" )
set ( COFFEE_ASIO_LIBRARY   "" CACHE FILEPATH "Coffee ASIO library" )
set ( COFFEE_GLEAM_LIBRARY  "" CACHE FILEPATH "Coffee GLeam library" )
set ( COFFEE_OPENAL_LIBRARY "" CACHE FILEPATH "Coffee OpenAL library" )

set ( COFFEE_SEARCH_PATHS
    /usr/local
    /usr
    /opt

    ~/Library/Frameworks
    /Library/Frameworks

    "${COFFEE_ROOT_DIR}"
    "$ENV{COFFEE_ROOT_DIR}"
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

find_library ( COFFEE_CORE_LIBRARY_TMP
    CoffeeCore

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

find_library ( COFFEE_ASIO_LIBRARY_TMP
    CoffeeASIO

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

find_library ( COFFEE_OPENVR_LIBRARY_TMP
    CoffeeOpenVR
    CoffeeVR

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

find_library ( COFFEE_OPENAL_LIBRARY_TMP
    CoffeeOpenAL

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

find_library ( COFFEE_GLEAM_LIBRARY_TMP
    CoffeeGLeam

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

if(COFFEE_INCLUDE_DIR_TMP)
    set (COFFEE_INCLUDE_DIR "${COFFEE_INCLUDE_DIR_TMP}")
endif()
if(COFFEE_CORE_LIBRARY_TMP)
    set (COFFEE_CORE_LIBRARY "${COFFEE_CORE_LIBRARY_TMP}")
endif()

if(COFFEE_ASIO_LIBRARY_TMP)
    set (COFFEE_ASIO_LIBRARY "${COFFEE_ASIO_LIBRARY_TMP}")
endif()

if(COFFEE_OCULUS_LIBRARY_TMP)
    set (COFFEE_OCULUS_LIBRARY "${COFFEE_OCULUS_LIBRARY_TMP}")
endif()

if(COFFEE_OPENVR_LIBRARY_TMP)
    set (COFFEE_OPENVR_LIBRARY "${COFFEE_OPENVR_LIBRARY_TMP}")
endif()

if(COFFEE_GLEAM_LIBRARY_TMP)
    set (COFFEE_GLEAM_LIBRARY "${COFFEE_GLEAM_LIBRARY_TMP}")
endif()

if(COFFEE_OPENAL_LIBRARY_TMP)
    set (COFFEE_OPENAL_LIBRARY "${COFFEE_OPENAL_LIBRARY_TMP}")
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Coffee

    REQUIRED_VARS
    COFFEE_CORE_LIBRARY
    COFFEE_INCLUDE_DIR
    COFFEE_GLEAM_LIBRARY
    COFFEE_OPENAL_LIBRARY
    )
