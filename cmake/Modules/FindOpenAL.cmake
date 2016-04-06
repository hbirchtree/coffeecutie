set ( OPENAL_SEARCH_PATHS
    /usr
    /usr/local
    ${OPENAL_ROOT}
    )

if(ANDROID)
    list ( APPEND OPENAL_SEARCH_PATHS
        /home/coffee/libs
        )
endif()

set ( OPENAL_INCLUDE_DIR "" CACHE PATH "OpenAL include directory" )
set ( OPENAL_LIBRARY "" CACHE FILEPATH "OpenAL library file" )

find_path ( OPENAL_INCLUDE_DIR_TMP
    al.h
    alc.h

    PATHS
    ${OPENAL_SEARCH_PATHS}

    PATH_SUFFIXES
    include
    include/AL
    )

find_library( OPENAL_LIBRARY_TMP
    openal
    OpenAL

    PATHS
    ${OPENAL_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib/${ANDROID_ABI}
    )

if(OPENAL_INCLUDE_DIR_TMP)
    set ( OPENAL_INCLUDE_DIR "${OPENAL_INCLUDE_DIR_TMP}")
endif()
if(OPENAL_LIBRARY_TMP)
    set ( OPENAL_LIBRARY "${OPENAL_LIBRARY_TMP}")
endif()

mark_as_advanced ( OPENAL_LIBRARY OPENAL_INCLUDE_DIR )
