# Core libraries, no external dependencies
add_subdirectory(core)
add_subdirectory(image)

# ASIO networking
if(COFFEE_BUILD_ASIO)
    add_subdirectory(asio)
endif()

# More here!
add_subdirectory(graphics)

add_subdirectory(windowing)

if(COFFEE_BUILD_CBLAM)
    add_subdirectory(blam)
endif()

# Requires OpenAL
if(COFFEE_BUILD_OPENAL)
    add_subdirectory(audio)
endif()

# Requires SDL2... obviously
if(COFFEE_BUILD_SDL2)
    add_subdirectory(sdl2)
endif()

# Compiled by user
if(COFFEE_BUILD_ASSIMP)
    add_subdirectory(assimp)
endif()

if(COFFEE_BUILD_CEF)
    add_subdirectory(cef)
endif()

if(COFFEE_BUILD_DBUS)
    add_subdirectory(misc/dbus)
endif()

coffee_bundle_includes ( "${CMAKE_CURRENT_SOURCE_DIR}/../include" )
