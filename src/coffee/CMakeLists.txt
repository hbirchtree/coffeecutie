option ( BUILD_ASIO     "Build with ASIO networking"                ON  )
option ( BUILD_ASSIMP   "Build with ASSIMP importer"                OFF )
option ( BUILD_AUDIO    "Build with audio support"                  ON  )
option ( BUILD_BLAM     "Build Blam! data readers"                  ON  )
option ( BUILD_CEF      "Build with Chromium Embedded Framework"    OFF )
option ( BUILD_OPENVR   "Build with OpenVR integration"             OFF )
option ( BUILD_SDL2     "Build with SDL2 graphics backend"          ON  )

option ( BUILD_HEADFUL  "Build with graphics/windowing support"     ON )

add_subdirectory(application)

add_subdirectory(core)
add_subdirectory(image)

if(BUILD_HEADFUL)
    add_subdirectory(graphics)
endif()

if(BUILD_SDL2)
    add_subdirectory(sdl2)
endif()

if(BUILD_HEADFUL)
    add_subdirectory(windowing)
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
