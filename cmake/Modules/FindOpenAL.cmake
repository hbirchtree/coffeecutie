# TODO: Write this Find file!
find_path ( OPENAL_INCLUDE_DIR
    AL/al.h
    AL/alc.h

    PATHS
    /usr
    /usr/local
    ${OPENAL_ROOT}

    PATH_SUFFIXES
    /include

    )

find_library( OPENAL_LIBRARY
    openal

    PATHS
    /usr/lib
    ${OPENAL_ROOT}/lib
    )

mark_as_advanced ( OPENAL_LIBRARY OPENAL_INCLUDE_DIR )
