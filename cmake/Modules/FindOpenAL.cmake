set ( OPENAL_SEARCH_PATHS
    /usr
    /usr/local
    ${OPENAL_ROOT}
    ${CMAKE_BINARY_DIR}/libs
	"C:/Program Files/OpenAL 1.1 SDK"
    "C:/Program Files (x86)/OpenAL 1.1 SDK"
	~/Library/Frameworks
	/Library/Frameworks
    )

if(ANDROID)
    list ( APPEND OPENAL_SEARCH_PATHS
        /home/coffee/libs
        )
endif()

#set ( OPENAL_INCLUDE_DIR CACHE PATH "OpenAL include directory" )
#set ( OPENAL_LIBRARY CACHE FILEPATH "OpenAL library file" )
#set ( OPENAL_EFX_LIBRARY CACHE FILEPATH "OpenAL EFX library file" )

find_path ( OPENAL_INCLUDE_DIR
    al.h
    alc.h

    PATH_SUFFIXES
    include
    include/AL

    PATHS
    ${OPENAL_SEARCH_PATHS}
    )

find_library ( OPENAL_LIBRARY
	NAMES
    openal
    OpenAL
	OpenAL32

    PATH_SUFFIXES
    lib
    lib/${ANDROID_ABI}
	libs/Win64

    PATHS
    ${OPENAL_SEARCH_PATHS}
    )

find_library ( OPENAL_EFX_LIBRARY
	NAMES
    EFX-Util

    PATH_SUFFIXES
    lib
    lib/${ANDROID_ABI}
	libs/Win64/EFX-Util_MT

    PATHS
    ${OPENAL_SEARCH_PATHS}
    )

set ( OPENAL_INCLUDE_DIR "${OPENAL_INCLUDE_DIR}" CACHE PATH "OpenAL include directory" FORCE )
set ( OPENAL_LIBRARY "${OPENAL_LIBRARY}" CACHE FILEPATH "OpenAL library file" FORCE )
set ( OPENAL_EFX_LIBRARY "${OPENAL_EFX_LIBRARY}" CACHE FILEPATH "OpenAL EFX library file" FORCE )

if(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)
	set ( OPENAL_FOUND ON )
endif()

mark_as_advanced(OPENAL_FOUND OPENAL_LIBRARY OPENAL_EFX_LIBRARY OPENAL_INCLUDE_DIR)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenAL REQUIRED_VARS OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
