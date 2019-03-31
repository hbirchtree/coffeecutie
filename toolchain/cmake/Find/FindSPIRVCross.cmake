find_path ( SPVCROSS_INCLUDE_DIR_TMP
    NAMES
    spirv_glsl.hpp

    PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    include/spirv_cross
    )

find_library ( SPVCROSS_CORE_LIBRARY_TMP
    NAMES spirv-cross-core

    PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    lib
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release
    "lib/${WINDOWS_ABI}"
    )

find_library ( SPVCROSS_CPP_LIBRARY_TMP
    NAMES spirv-cross-cpp

    PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    lib
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release
    "lib/${WINDOWS_ABI}"
    )

find_library ( SPVCROSS_UTIL_LIBRARY_TMP
    NAMES spirv-cross-util

    PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    lib
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release
    "lib/${WINDOWS_ABI}"
    )

find_library ( SPVCROSS_GLSL_LIBRARY_TMP
    NAMES spirv-cross-glsl

    PATHS
    /usr
    /usr/local
    ${CMAKE_SOURCE_DIR}/libs
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    lib
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}/Release
    "lib/${WINDOWS_ABI}"
    )

if(NOT TARGET spvcross)
    add_library ( spvcross STATIC IMPORTED )
endif()

set_target_properties( spvcross PROPERTIES
    IMPORTED_LOCATION ${SPVCROSS_GLSL_LIBRARY_TMP}

    INTERFACE_LINK_LIBRARIES
    ${SPVCROSS_CORE_LIBRARY_TMP}
    ${SPVCROSS_CPP_LIBRARY_TMP}
    ${SPVCROSS_UTIL_LIBRARY_TMP}

    INTERFACE_INCLUDE_DIRECTORIES ${SPVCROSS_INCLUDE_DIR_TMP}
    )

if(SPVCROSS_CORE_LIBRARY_TMP AND SPVCROSS_INCLUDE_DIR_TMP)
    set ( SPVCROSS_FOUND TRUE )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS( SPIRVCross
    REQUIRED_VARS
    SPVCROSS_INCLUDE_DIR_TMP
    SPVCROSS_LIBRARY_TMP
    SPVCROSS_FOUND
    )
