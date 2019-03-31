set ( OPENVR_SEARCH_PATHS
    /usr
    /usr/local
    ${OPENVR_ROOT}
    ${COFFEE_EXT_LIBRARY_DIR}/openvr
    ${CMAKE_BINARY_DIR}/libs
    )

if(ANDROID)
    list ( APPEND OPENVR_SEARCH_PATHS
        /home/coffee/libs
        )
endif()

find_path ( OPENVR_INCLUDE_DIR
    openvr.h
    openvr_capi.h

    PATHS
    ${OPENVR_SEARCH_PATHS}

    PATH_SUFFIXES
    include
    headers
    Headers
    )

set ( TARGET_PATH "lib" )
set ( TARGET_ARCH )

if(WIN32)
	set ( TARGET_PATH "lib" )
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	set ( TARGET_ARCH "win64" )
	else()
	set ( TARGET_ARCH "win32" )
	endif()
elseif(APPLE)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	set ( TARGET_ARCH "osx64" )
	else()
	set ( TARGET_ARCH "osx32" )
	endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set ( TARGET_ARCH "linux64" )
	else()
	set ( TARGET_ARCH "linux32" )
	endif()
endif()

find_library( OPENVR_LIBRARY
    openvr_api

    PATH_SUFFIXES
    lib
    ${TARGET_PATH}/${TARGET_ARCH}

    PATHS
    ${OPENVR_SEARCH_PATHS}
    )

set ( OPENVR_INCLUDE_DIR "${OPENVR_INCLUDE_DIR}" CACHE PATH "OpenVR include directory" )
set ( OPENVR_LIBRARY "${OPENVR_LIBRARY}" CACHE FILEPATH "OpenVR library file" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVR REQUIRED_VARS OPENVR_LIBRARY OPENVR_INCLUDE_DIR)
