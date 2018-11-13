if(NOT EMSCRIPTEN)
    if(NOT TARGET SDL2)
        find_package ( SDL2 REQUIRED )
    endif()

    set ( LIBS
        SDL2
        )
endif()

if(TARGET CoffeeGLeamCommon)
    list ( APPEND LIBS CoffeeGLeamCommon )
endif()

coffee_library (
    TARGET CoffeeSDL2
    SOURCES
    private/sdl2system.cpp
    private/windowing/csdl2_dialogs.cpp
    private/graphics/csdl2_basic_renderer.cpp
    private/graphics/csdl2_gl_renderer.cpp
    private/input/csdl2_eventhandler.cpp
    private/windowing/csdl2_window.cpp
    private/windowing/sdl2helpers.cpp
    private/input/evhandlers/sdl2eventhandlers.cpp

    LIBRARIES CoffeeCore ${LIBS}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_compile_definitions ( CoffeeSDL2 PUBLIC
    -DCOFFEE_USE_SDL2=1
    )

if(NOT EMSCRIPTEN)
    target_include_directories ( CoffeeSDL2 PRIVATE
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
        )

    coffee_bundle_includes( ${SDL2_INCLUDE_DIR} )
    coffee_bundle_library( ${SDL2_LIBRARY} )
endif()
