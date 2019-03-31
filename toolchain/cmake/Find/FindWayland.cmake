set ( WAYLAND_SEARCH_PATHS
    /usr
    /usr/local
    )

find_path ( WAYLAND_INCLUDE_DIR
    wayland-client.h wayland-cursor.h wayland-egl.h
    PATH_SUFFIXES include
    PATHS ${WAYLAND_SEARCH_PATHS}
    )

find_library ( WAYLAND_CLIENT_LIB_TMP
    NAMES wayland-client
    PATH_SUFFIXES lib
    PATHS ${WAYLAND_SEARCH_PATHS}
    )

find_library ( WAYLAND_CURSOR_LIB_TMP
    NAMES wayland-cursor
    PATH_SUFFIXES lib
    PATHS ${WAYLAND_SEARCH_PATHS}
    )

find_library ( WAYLAND_EGL_LIB_TMP
    NAMES wayland-egl
    PATH_SUFFIXES lib
    PATHS ${WAYLAND_SEARCH_PATHS}
    )

if(WAYLAND_CLIENT_LIB_TMP AND WAYLAND_CURSOR_LIB_TMP AND WAYLAND_EGL_LIB_TMP)
    set ( WAYLAND_LIBRARIES "${WAYLAND_CLIENT_LIB_TMP};${WAYLAND_CURSOR_LIB_TMP};${WAYLAND_EGL_LIB_TMP}"
        CACHE STRING "" )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Wayland
    REQUIRED_VARS
    WAYLAND_LIBRARIES
    WAYLAND_INCLUDE_DIR)
