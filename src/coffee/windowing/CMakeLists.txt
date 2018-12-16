add_library ( WindowingInclude INTERFACE )

target_include_directories ( WindowingInclude INTERFACE

    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

coffee_bundle(
    HEADER_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

install (
    TARGETS WindowingInclude
    EXPORT ${PROJECT_NAME}
    )

register_library ( WindowingInclude
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

set ( GLAD_SRCS
    )

set ( Renderer_SRCS
    private/renderer.cpp
    private/renderer_internal.cpp
    private/stub/stub.cpp
    )

if(BUILD_OPENGL)
    set ( RENDERER_LIBS
        WindowX11
        WindowNDKWindow

        ContextGLX
        ContextEGL

        BindingGLAD

        GLeamRHI
        )
elseif(GAMECUBE)
    set ( RENDERER_LIBS
        WindowOGC
        )
endif()

if(BUILD_SDL2)
    list ( APPEND RENDERER_LIBS SDL2Backend )
endif()

if(BUILD_OPENGL)
    coffee_library (
        TARGET ContextEGL
        SOURCES
        private/graphics/egl/egl_renderer.cpp
        LIBRARIES Core WindowingInclude GLeamCommon
        )

    if(BUILD_GLES AND NOT APPLE)
        target_link_libraries ( ContextEGL PUBLIC
            EGL
            )
    endif()

    coffee_library (
        TARGET ContextGLX
        SOURCES
        private/graphics/glx/glx_renderer.cpp
        LIBRARIES Core GLeamCommon WindowingInclude
        )

    coffee_library (
        TARGET BindingGLAD
        SOURCES
        private/binding/glad/gleamrenderer.cpp
        LIBRARIES Core GLeamCommon WindowingInclude
        )

    if(BUILD_GLES AND NOT BUILD_GLES_20)
        target_sources ( BindingGLAD PRIVATE
            private/binding/glad/glad_es/glad.c
            )
    elseif(NOT BUILD_GLES)
        target_sources ( BindingGLAD PRIVATE
            private/binding/glad/glad/glad.c
            )
    endif()
endif(BUILD_OPENGL)

coffee_library (
    TARGET WindowOGC
    SOURCES
    private/windowing/ogc/ogc_window.cpp
    LIBRARIES Core WindowingInclude
    )

coffee_library (
    TARGET WindowX11
    SOURCES
    private/windowing/x11/x11_window.cpp
    LIBRARIES Core GLeamCommon WindowingInclude
    )

coffee_library (
    TARGET WindowDMX
    SOURCES
    private/windowing/dispmanx/dispmanx_window.cpp
    LIBRARIES Core WindowingInclude
    )

if(APPLE AND IOS)
    coffee_library (
        TARGET WindowGLKit
        SOURCES
        private/windowing/glkit/glk_window.cpp
        private/graphics/ceagl/private/eagl_bind.mm
        private/graphics/ceagl/private/EGLView.m
        private/windowing/glkit/glk_window.mm
        private/windowing/glkit/AppDelegate.m
        LIBRARIES
        Core
        WindowingInclude
        GLeamCommon
        "-framework CoreGraphics"
        "-framework QuartzCore"
        )

    # Add CEAGL layer, to provide EGL
    target_include_directories ( BindingGLAD PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )
    target_include_directories ( WindowGLKit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )
    target_include_directories ( ContextEGL PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )

    target_link_libraries ( WindowGLKit PUBLIC
        "-framework CoreMotion"
        "-framework GameController"
        "-framework OpenGLES"
        "-framework UIKit"
        "-framework GLKit"
        )
endif()

coffee_library (
    TARGET WindowNDKWindow
    SOURCES
    private/windowing/ndkwindow/ndk_window.cpp
    LIBRARIES Core WindowingInclude
    )

coffee_library (
    TARGET RendererDefault
    SOURCES ${Renderer_SRCS}
    LIBRARIES Core WindowingInclude ${RENDERER_LIBS}
    )

if(APPLE AND IOS)
    target_link_libraries ( RendererDefault PUBLIC
        WindowGLKit
        )
endif()

if(BUILD_OPENGL)
    target_compile_definitions ( RendererDefault
        PUBLIC
        -DCOFFEE_ENABLE_OPENGL
        )
endif(BUILD_OPENGL)

if(NOT EMSCRIPTEN AND BUILD_SDL2 AND BUILD_OPENGL)
    find_package(SDL2 REQUIRED)
    target_include_directories ( BindingGLAD PRIVATE
        ${SDL2_INCLUDE_DIR}
        ${STANDARDLIBS_INCLUDE_DIR}
        )
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID
        AND NOT EMSCRIPTEN AND NOT BUILD_SDL2)
    target_compile_definitions ( RendererDefault PUBLIC
        -DCOFFEE_LINUX_LIGHTWEIGHT_WM
        )

    find_package ( X11 QUIET )

    if(NOT BUILD_GLES)
        target_link_libraries ( ContextGLX PUBLIC
            Xrender GL
            ${X11_LIBRARIES}
            )
        target_include_directories( ContextGLX PRIVATE
            ${X11_INCLUDE_DIR}
            )
    endif()

    target_link_libraries ( WindowX11 PUBLIC
        ${X11_LIBRARIES}
        )
    target_include_directories( WindowX11 PRIVATE
        ${X11_INCLUDE_DIR}
        )
endif()
