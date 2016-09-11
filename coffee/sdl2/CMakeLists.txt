set ( CoffeeSDL2_SRCS
    private/sdl2system.cpp

    private/windowing/csdl2_dialogs.cpp
    private/graphics/csdl2_basic_renderer.cpp
    private/graphics/csdl2_gl_renderer.cpp
    private/input/csdl2_eventhandler.cpp
    private/windowing/csdl2_window.cpp

    private/input/evhandlers/sdl2eventhandlers.cpp
    private/windowing/sdl2helpers.cpp
    )

set ( CoffeeSDL2Audio_SRCS
    private/audio/csdl2_audio.cpp
    )

find_package ( SDL2 REQUIRED)

#if(NOT SDL2_FOUND)
#    message(FATAL_ERROR "Failed to locate SDL2")
#endif()

include_directories ( SYSTEM
    ${SDL2_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeSDL2 "${CoffeeSDL2_SRCS}" )


if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    find_package ( X11 REQUIRED )
    find_package ( Wayland QUIET )
    find_package ( Mir QUIET )

    if(X11_FOUND)
        target_link_libraries(CoffeeSDL2
            ${X11_LIBRARIES}
            )
        include_directories(
            ${X11_INCLUDE_DIR}
            )
    endif()
    if(WAYLAND_FOUND)
        target_link_libraries(CoffeeSDL2
            ${WAYLAND_LIBRARIES}
            )
        include_directories(
            ${WAYLAND_INCLUDE_DIR}
            )
    endif()
    if(MIR_FOUND)
        target_link_libraries(CoffeeSDL2
            ${MIR_LIBRARIES}
            )
        include_directories(
            ${MIR_INCLUDE_DIR}
            )
    endif()
endif()

target_link_libraries( CoffeeSDL2
    CoffeeCore
    ${SDL2_LIBRARY}
    )

coffee_add_library ( CoffeeSDL2Audio "${CoffeeSDL2Audio_SRCS}" )
