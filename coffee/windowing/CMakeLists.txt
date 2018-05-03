set ( X11_SRCS
    private/windowing/x11/x11_window.cpp
    )

set ( DMX_SRCS
    private/windowing/dispmanx/dispmanx_window.cpp
    )

set ( EGL_SRCS
    private/graphics/egl/egl_renderer.cpp
    )

set ( GLX_SRCS
    private/graphics/glx/glx_renderer.cpp
    )

set ( GLKit_SRCS
    private/windowing/glkit/glk_window.cpp
    )

set ( NDKWindow_SRCS
    private/windowing/ndkwindow/ndk_window.cpp
    )

set ( OGC_SRCS
    private/windowing/ogc/ogc_window.cpp
    )

if(APPLE AND IOS)
    list ( APPEND GLKit_SRCS
        private/graphics/ceagl/private/eagl_bind.mm
        private/graphics/ceagl/private/EGLView.m
        private/windowing/glkit/glk_window.mm
        private/windowing/glkit/AppDelegate.m
        )
endif()

set ( GLAD_SRCS
    private/binding/glad/gleamrenderer.cpp
    )

set ( Renderer_SRCS
    private/renderer.cpp
    private/renderer_internal.cpp
    private/stub/stub.cpp
    )

if(COFFEE_BUILD_GLES AND NOT COFFEE_BUILD_GLES_20)
    list(APPEND GLAD_SRCS private/binding/glad/glad_es/glad.c)
elseif(NOT COFFEE_BUILD_GLES)
    list(APPEND GLAD_SRCS private/binding/glad/glad/glad.c)
endif()

if(COFFEE_BUILD_OPENGL)
    set ( RENDERER_LIBS
        CoffeeWindow_X11
        CoffeeWindow_DMX
        CoffeeWindow_GLKit
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

if(COFFEE_BUILD_SDL2)
    list ( APPEND RENDERER_LIBS CoffeeSDL2 )
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"
        AND NOT ANDROID AND NOT NACL AND NOT EMSCRIPTEN
        AND NOT COFFEE_BUILD_SDL2)
endif()

if(COFFEE_BUILD_OPENGL)
    coffee_library (
        TARGET CoffeeContext_EGL
        SOURCES ${EGL_SRCS}
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/graphics/egl
        )

    coffee_library (
        TARGET CoffeeContext_GLX
        SOURCES ${GLX_SRCS}
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/graphics/glx
        )

    coffee_library (
        TARGET CoffeeBinding_GLAD
        SOURCES ${GLAD_SRCS}
        LIBRARIES CoffeeGLeamCommon
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/binding/glad
        )

    target_link_libraries ( CoffeeBinding_GLAD
        PRIVATE
        CoffeeCore
        )

endif(COFFEE_BUILD_OPENGL)

coffee_library (
    TARGET CoffeeWindow_OGC
    SOURCES ${OGC_SRCS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/windowing/ogc
    )

coffee_library (
    TARGET CoffeeWindow_X11
    SOURCES ${X11_SRCS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/windowing/x11
    )

coffee_library (
    TARGET CoffeeWindow_DMX
    SOURCES ${DMX_SRCS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/windowing/dispmanx
    )

coffee_library (
    TARGET CoffeeWindow_GLKit
    SOURCES ${GLKit_SRCS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/windowing/glkit
    )

coffee_library (
    TARGET CoffeeWindow_NDKWindow
    SOURCES ${NDKWindow_SRCS}
    HEADER_DIRS
    ${CMAKE_SOURCE_DIR}/include/coffee/windowing/windowing/ndkwindow
    )

coffee_library (
    TARGET CoffeeRenderer_Default
    SOURCES ${Renderer_SRCS}
    LIBRARIES ${RENDERER_LIBS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/windowing/renderer
    )

target_link_libraries( CoffeeWindow_NDKWindow
    PRIVATE
    CoffeeCore
    )

if(APPLE AND IOS)
    target_include_directories ( CoffeeBinding_GLAD
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )
    target_include_directories ( CoffeeWindow_GLKit
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )
    target_include_directories ( CoffeeContext_EGL
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics/ceagl/include
        )
endif()

if(COFFEE_BUILD_OPENGL)
    target_compile_definitions ( CoffeeRenderer_Default
        PUBLIC
        -DCOFFEE_ENABLE_OPENGL
        )

    target_link_libraries( CoffeeContext_EGL
        PRIVATE
        CoffeeCore
        )
endif(COFFEE_BUILD_OPENGL)

if(NOT EMSCRIPTEN AND COFFEE_BUILD_SDL2 AND COFFEE_BUILD_OPENGL)
    find_package(SDL2 REQUIRED)
    target_include_directories ( CoffeeBinding_GLAD
        PRIVATE
        ${SDL2_INCLUDE_DIR}
        ${STANDARDLIBS_INCLUDE_DIR}
        )
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"
        AND NOT ANDROID AND NOT NACL AND NOT EMSCRIPTEN
        AND NOT COFFEE_BUILD_SDL2)
    find_package ( X11 QUIET )

    if(NOT COFFEE_BUILD_GLES)
        target_link_libraries ( CoffeeContext_GLX
            PUBLIC
            Xrender GL
            ${X11_LIBRARIES}
            )
        target_include_directories( CoffeeContext_GLX
            PRIVATE
            ${X11_INCLUDE_DIR}
            )
    endif()

    target_link_libraries ( CoffeeWindow_X11
        PUBLIC
        ${X11_LIBRARIES}
        )
    target_include_directories( CoffeeWindow_X11
        PRIVATE
        ${X11_INCLUDE_DIR}
        )
endif()
