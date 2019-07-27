set ( SHADERC_PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${SHADERC_DIR}
    )

find_path ( SHADERC_INCLUDE_DIR_TMP
    NAMES
    shaderc/shaderc.h
    shaderc/shaderc.hpp

    PATHS ${SHADERC_PATHS}

    PATH_SUFFIXES
    include
    )

find_library ( SHADERC_LIBRARY_TMP
    NAMES shaderc_combined

    PATHS ${SHADERC_PATHS}

    PATH_SUFFIXES
    lib
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release
    "lib/${WINDOWS_ABI}"
    )

if(NOT TARGET shaderc)
    add_library ( shaderc STATIC IMPORTED )
endif()

set_target_properties( shaderc PROPERTIES
    IMPORTED_LOCATION ${SHADERC_LIBRARY_TMP}
    INTERFACE_INCLUDE_DIRECTORIES ${SHADERC_INCLUDE_DIR_TMP}
    )

if(SHADERC_LIBRARY_TMP AND SHADERC_INCLUDE_DIR_TMP)
    set ( SHADERC_FOUND TRUE )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(shaderc
    REQUIRED_VARS
    SHADERC_INCLUDE_DIR_TMP
    SHADERC_LIBRARY_TMP
    SHADERC_FOUND
    )
