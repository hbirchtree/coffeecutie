# Core libraries, no external dependencies
add_subdirectory(core)
add_subdirectory(image)

# ASIO networking
if(COFFEE_BUILD_OPENSSL)
    add_subdirectory(asio)
endif()

# More here!
add_subdirectory(graphics)

if(COFFEE_BUILD_CBLAM)
    add_subdirectory(blam)
endif()

# Requires OpenAL
if(COFFEE_BUILD_OPENAL)
    add_subdirectory(audio)
endif()
# Requires SDL2
if(COFFEE_BUILD_SDL2)
    add_subdirectory(sdl2)
endif()

# Compiled by user
if(COFFEE_BUILD_ASSIMP)
    add_subdirectory(assimp)
endif()
# Should be user-compiled for license reasons
if(COFFEE_BUILD_FFMEDIA)
    add_subdirectory(media)
endif()

# Requires OpenVR libraries from Valve
#if(COFFEE_BUILD_OPENVR)
#    add_subdirectory(openvr)
#endif()

## Requires Oculus SDK
#if(COFFEE_BUILD_OCULUSVR)
#    add_subdirectory(oculusvr)
#endif()

if(COFFEE_BUILD_CEF)
    add_subdirectory(cef)
endif()
