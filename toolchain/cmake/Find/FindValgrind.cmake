find_program ( VALGRIND_PROGRAM
    valgrind

    PATHS
    /usr
    /usr/local
    /opt/valgrind

    PATH_SUFFIXES
    bin bin64
    )

set ( VALGRIND_PROGRAM "${VALGRIND_PROGRAM}" CACHE FILEPATH "Valgrind executable" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Valgrind REQUIRED_VARS VALGRIND_PROGRAM)
