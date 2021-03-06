option ( BUILD_ASIO     "Build with ASIO networking"                ON  )
option ( BUILD_ASSIMP   "Build with ASSIMP importer"                OFF )
option ( BUILD_AUDIO    "Build with audio support"                  ON  )
option ( BUILD_BLAM     "Build Blam! data readers"                  ON  )
option ( BUILD_CEF      "Build with Chromium Embedded Framework"    OFF )
option ( BUILD_OPENVR   "Build with OpenVR integration"             OFF )
option ( BUILD_SDL2     "Build with SDL2 graphics backend"          ON  )
option ( BUILD_COMPONENT_APP "Build component-based application"    ON  )

option ( BUILD_EGL      "Build EGL component (Android/Linux/Window only)" OFF )
option ( BUILD_X11      "Build X11 components (Linux/macOS only)"         OFF )
option ( BUILD_GLX      "Build X11/GLX components (Linux only)"           OFF )
option ( BUILD_GLKIT    "Build GLKit components (macOS/iOS only)"         OFF )
option ( BUILD_LIBOGC   "Build libogc components (GameCube/Wii only)"     OFF )
option ( BUILD_ANATIVE  "Build ANativeWindow components (Android only)"   OFF )

option ( BUILD_HEADFUL  "Build with graphics/windowing support"     ON )

add_subdirectory(application)

add_subdirectory(core)
add_subdirectory(image)

set ( CEAGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/windowing/private/graphics/ceagl" )

if(BUILD_HEADFUL)
    add_subdirectory(graphics)
endif()

if(BUILD_COMPONENT_APP)
    add_subdirectory(comp_app)

    add_library ( ComponentBundle INTERFACE )
    add_library ( MobileBundle INTERFACE )

    if(BUILD_OPENGL)
        target_compile_definitions ( ComponentBundle INTERFACE
            -DSELECT_API_OPENGL=1
            )

        if(BUILD_GLEAM)
            target_link_libraries ( ComponentBundle INTERFACE GLVersion )
        endif()

        if(NOT BUILD_GLES)
            add_subdirectory(graphics/comp_app)
            target_link_libraries ( ComponentBundle INTERFACE GLADComponent )
        endif()

        if(BUILD_EGL)
            add_subdirectory(graphics/egl_comp)
            target_link_libraries ( ComponentBundle INTERFACE EGLComponent )
        endif()
    endif()
    if(BUILD_X11)
        add_subdirectory(windowing/x11_comp)
        target_link_libraries ( ComponentBundle INTERFACE X11Component )
    endif()
    if(BUILD_GLX)
        add_subdirectory(graphics/glx_comp)
        target_link_libraries ( ComponentBundle INTERFACE GLXComponent )
    endif()
    if(BUILD_GLKIT)
        add_subdirectory(windowing/glkit_comp)
        add_subdirectory(uikit)
        target_link_libraries ( ComponentBundle INTERFACE GLKitComponent )
        target_link_libraries ( MobileBundle INTERFACE UIKitGestures )
    endif()
    if(BUILD_LIBOGC)
        add_subdirectory(windowing/cog_comp)
        target_link_libraries ( ComponentBundle INTERFACE CogComponent )
    endif()
    if(BUILD_ANATIVE)
        add_subdirectory(windowing/anative_comp)
        target_link_libraries ( ComponentBundle INTERFACE ANativeComponent )
    endif()
    if(BUILD_SDL2)
        add_subdirectory(sdl2_comp)
        target_link_libraries ( ComponentBundle INTERFACE SDL2Components )
    endif()

    target_link_libraries ( ComponentBundle INTERFACE MobileBundle )

    add_export ( MobileBundle    "" )
    add_export ( ComponentBundle "" )
endif()

if(BUILD_ASIO)
    add_subdirectory(asio)
endif()

if(BUILD_ASSIMP)
    add_subdirectory(assimp)
endif()

if(BUILD_BLAM)
    add_subdirectory(blam)
endif()

if(BUILD_CEF)
    add_subdirectory(cef)
endif()

if(BUILD_AUDIO)
    add_subdirectory(audio)
endif()


coffee_library (
    TARGET OnlineService
    SOURCES misc/online_service.cpp
    LIBRARIES Core
    )
