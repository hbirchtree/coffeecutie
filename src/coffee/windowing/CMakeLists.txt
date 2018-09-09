set ( GLAD_SRCS
    )

set ( Renderer_SRCS
    private/renderer.cpp
    private/renderer_internal.cpp
    private/stub/stub.cpp
    )

if(BUILD_OPENGL)
    set ( RENDERER_LIBS
        CoffeeWindow_X11
        CoffeeWindow_NDKWindow

        CoffeeContext_GLX
        CoffeeContext_EGL

        CoffeeBinding_GLAD

        CoffeeGLeamRHI
        )
elseif(GAMECUBE)
    set ( RENDERER_LIBS
        CoffeeWindow_OGC
        )
endif()

if(BUILD_SDL2)
    list ( APPEND RENDERER_LIBS CoffeeSDL2 )
endif()

if(BUILD_OPENGL)
    coffee_library (
        TARGET CoffeeContext_EGL
        SOURCES
        private/graphics/egl/egl_renderer.cpp
        LIBRARIES
        CoffeeCore
        EGL
        )

    coffee_library (
        TARGET CoffeeContext_GLX
        SOURCES
        private/graphics/glx/glx_renderer.cpp
        LIBRARIES CoffeeCore
        )

    coffee_library (
        TARGET CoffeeBinding_GLAD
        SOURCES
        private/binding/glad/gleamrenderer.cpp
        LIBRARIES CoffeeCore CoffeeGLeamCommon
        )

    if(BUILD_GLES AND NOT BUILD_GLES_20)
        target_sources ( CoffeeBinding_GLAD PRIVATE
            private/binding/glad/glad_es/glad.c
            )
    elseif(NOT BUILD_GLES)
        target_sources ( CoffeeBinding_GLAD PRIVATE
            private/binding/glad/glad/glad.c
            )
    endif()
endif(BUILD_OPENGL)

coffee_library (
    TARGET CoffeeWindow_OGC
    SOURCES
    private/windowing/ogc/ogc_window.cpp
    LIBRARIES CoffeeCore
    )

coffee_library (
    TARGET CoffeeWindow_X11
    SOURCES
    private/windowing/x11/x11_window.cpp
    LIBRARIES CoffeeCore
    )

coffee_library (
    TARGET CoffeeWindow_DMX
    SOURCES
    private/windowing/dispmanx/dispmanx_window.cpp
    LIBRARIES CoffeeCore
    )

if(APPLE)
    coffee_library (
        TARGET CoffeeWindow_GLKit
        SOURCES
        private/windowing/glkit/glk_window.cpp
        private/graphics/ceagl/private/eagl_bind.mm
        private/graphics/ceagl/private/EGLView.m
        private/windowing/glkit/glk_window.mm
        private/windowing/glkit/AppDelegate.m
        LIBRARIES
        CoffeeCore
        "-framework CoreGraphics"
        "-framework QuartzCore"
        )

    if(IOS)
        # Add CEAGL layer, to provide EGL
        target_include_directories ( CoffeeBinding_GLAD PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
            )
        target_include_directories ( CoffeeWindow_GLKit PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
            )
        target_include_directories ( CoffeeContext_EGL PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
            )

        target_link_libraries ( CoffeeWindow_GLKit PUBLIC
            "-framework CoreMotion"
            "-framework GameController"
            "-framework OpenGLES"
            "-framework UIKit"
            "-framework GLKit"
            )
    else()
        target_link_libraries ( CoffeeWindow_GLKit PUBLIC
            "-framework OpenGL"
            "-framework AppKit"
            )
    endif()
endif()

coffee_library (
    TARGET CoffeeWindow_NDKWindow
    SOURCES
    private/windowing/ndkwindow/ndk_window.cpp
    LIBRARIES CoffeeCore
    )

coffee_library (
    TARGET CoffeeRenderer_Default
    SOURCES ${Renderer_SRCS}
    LIBRARIES CoffeeCore ${RENDERER_LIBS}
    )

if(APPLE)
    target_link_libraries ( CoffeeRenderer_Default PUBLIC
        CoffeeWindow_GLKit
        )
endif()

if(BUILD_OPENGL)
    target_compile_definitions ( CoffeeRenderer_Default
        PUBLIC
        -DCOFFEE_ENABLE_OPENGL
        )
endif(BUILD_OPENGL)

if(NOT EMSCRIPTEN AND BUILD_SDL2 AND BUILD_OPENGL)
    find_package(SDL2 REQUIRED)
    target_include_directories ( CoffeeBinding_GLAD PRIVATE
        ${SDL2_INCLUDE_DIR}
        ${STANDARDLIBS_INCLUDE_DIR}
        )
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID
        AND NOT EMSCRIPTEN AND NOT BUILD_SDL2)
    target_compile_definitions ( CoffeeRenderer_Default PUBLIC
        -DCOFFEE_LINUX_LIGHTWEIGHT_WM
        )

    find_package ( X11 QUIET )

    if(NOT BUILD_GLES)
        target_link_libraries ( CoffeeContext_GLX PUBLIC
            Xrender GL
            ${X11_LIBRARIES}
            )
        target_include_directories( CoffeeContext_GLX PRIVATE
            ${X11_INCLUDE_DIR}
            )
    endif()

    target_link_libraries ( CoffeeWindow_X11 PUBLIC
        ${X11_LIBRARIES}
        )
    target_include_directories( CoffeeWindow_X11 PRIVATE
        ${X11_INCLUDE_DIR}
        )
endif()
