option ( BUILD_OPENGL "Target OpenGL API" ON )
option ( BUILD_GRAPHICS_COMMON "Build common graphics library" ON )

option ( BUILD_GX "Target GX API for Gekko" OFF )

if(BUILD_OPENGL)
    # GLEAM supports OpenGL 3.3 -> 4.6 and OpenGL ES 2.0 -> 3.2
    option ( BUILD_GLEAM "Build with GLEAM graphics backend" ON )

    add_subdirectory(binding)

    if(BUILD_GLEAM)
        add_subdirectory(apis/gleam)
    endif()

    add_subdirectory(glscreenshot)
endif()

if(BUILD_GX)
    add_subdirectory(apis/gx)
endif()

if(BUILD_GRAPHICS_COMMON)
    add_subdirectory(common)
endif()
