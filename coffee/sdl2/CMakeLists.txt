set ( CoffeeSDL2_SRCS
    private/sdl2system.cpp

    private/windowing/csdl2_dialogs.cpp

    private/graphics/csdl2_basic_renderer.cpp

    private/graphics/csdl2_gl_renderer.cpp

    private/input/csdl2_eventhandler.cpp

    private/windowing/csdl2_window.cpp

    private/windowing/sdl2helpers.cpp
    private/input/evhandlers/sdl2eventhandlers.cpp
    )

set ( CoffeeSDL2Audio_SRCS
    private/audio/csdl2_audio.cpp
    )

if(NOT EMSCRIPTEN)
    if(NOT TARGET SDL2)
        find_package ( SDL2 REQUIRED )
    endif()

    set ( LIBS
        SDL2
        ${SDL2_LIBRARIES}
        )
endif()

coffee_library (
    TARGET CoffeeSDL2
    SOURCES ${CoffeeSDL2_SRCS}
    LIBRARIES ${LIBS}
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/sdl2
    )

target_link_libraries ( CoffeeSDL2
    PRIVATE
    CoffeeCore
    )

target_include_directories ( CoffeeSDL2
    PRIVATE
    $<BUILD_INTERFACE:${STANDARDLIBS_INCLUDE_DIR}>
    )

if(NOT EMSCRIPTEN)
    target_include_directories ( CoffeeSDL2
        PRIVATE
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
        )

    coffee_bundle_includes( ${SDL2_INCLUDE_DIR} )
    coffee_bundle_library( ${SDL2_LIBRARY} )
endif()
