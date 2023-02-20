option ( BUILD_EGL_DOUBLE_GETPROC "Lookup GL functions in EGL *and* GLES, used as a workaround for odd EGL implementations, eg. ImgTec" OFF )
option ( BUILD_EGL_STATIC "Use EGL_static library, eg. for Raspberry Pi" OFF )

coffee_library(
    TARGET EGLComponent
    SOURCES private/egl_comp.cpp
    LIBRARIES ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

if(BUILD_GLKIT)
    target_link_libraries ( EGLComponent PUBLIC CEAGL )
elseif(BUILD_EGL_STATIC)
    find_library ( EGL_STATIC_LIB EGL_static )
    target_link_libraries ( EGLComponent PUBLIC
        ${EGL_STATIC_LIB}
        )
else()
    find_library ( EGL_LIB EGL )
    target_link_libraries ( EGLComponent PUBLIC ${EGL_LIB} )
endif()

if(BUILD_EGL_DOUBLE_GETPROC)
    #
    # Workaround for ImTec's weird EGL loader
    # It will not let you load the core functions, only extensions
    # Work around this by first looking up the symbol in GLESv2, then EGL
    #
    target_compile_definitions( EGLComponent PUBLIC
        -DEGL_COMP_DOUBLE_GETPROC
        )
endif()
