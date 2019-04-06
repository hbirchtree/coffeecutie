if(NOT EMSCRIPTEN)
    if(NOT TARGET SDL2)
        if(ANDROID)
            dependency_get (
                REQUIRED
                SOURCE hbirchtree/native-library-bundle
                TAG ${NATIVE_LIB_BUNDLE_TAG}
                NAMES sdl2
                )
        endif()

        find_package ( SDL2 REQUIRED )
    endif()

    set ( LIBS
        SDL2
        )
endif()

if(TARGET GLeamCommon)
    list ( APPEND LIBS GLeamCommon )
endif()

coffee_library (
    TARGET SDL2Backend
    SOURCES
    private/sdl2system.cpp
    private/graphics/csdl2_basic_renderer.cpp
    private/graphics/csdl2_gl_renderer.cpp
    private/input/csdl2_eventhandler.cpp
    private/windowing/csdl2_window.cpp
    private/windowing/sdl2helpers.cpp
    private/input/evhandlers/sdl2eventhandlers.cpp

    LIBRARIES Core ${LIBS}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:libs/include>
    )

target_compile_definitions ( SDL2Backend PUBLIC
    -DCOFFEE_USE_SDL2=1
    )

if(NOT EMSCRIPTEN)
    target_include_directories ( SDL2Backend PRIVATE
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
        )

    coffee_bundle(
        DESTINATION libs
        HEADER_DIRECTORIES ${SDL2_INCLUDE_DIR}
        LIBRARIES ${SDL2_LIBRARY}
        )
endif()
