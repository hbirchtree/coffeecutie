option ( BUILD_OPENGL "Target OpenGL API" ON )

if(BUILD_OPENGL)
    # GLEAM supports OpenGL 3.3 -> 4.6 and OpenGL ES 2.0 -> 3.2
    option ( BUILD_GLEAM "Build with GLEAM graphics backend" ON )

    add_subdirectory(binding)

    if(BUILD_GLEAM)
        add_subdirectory(apis/gleam)
    endif()
endif()

add_subdirectory(common)
