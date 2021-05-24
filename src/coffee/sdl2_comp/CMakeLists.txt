find_package ( SDL2 CONFIG REQUIRED )
set ( LIBS
    SDL2::SDL2-static
    )

coffee_library (
    TARGET SDL2Components
    SOURCES
    private/sdl2_components.cpp

    LIBRARIES
    ComponentApp
    ${LIBS}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

if(NOT EMSCRIPTEN)
    target_include_directories ( SDL2Components PRIVATE
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}/SDL2>
        )
else()
    target_compile_options ( SDL2Components PUBLIC
            "-s " USE_SDL=2
       )
    target_link_options ( SDL2Components PUBLIC
            "-s " USE_SDL=2
       )
endif()

if(RASPBERRY)
    target_link_libraries ( SDL2Components PUBLIC
        bcm_host
        vchiq_arm
        vcos
        EGL
        GLESv2
        )
endif()
