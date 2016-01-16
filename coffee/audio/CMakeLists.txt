find_package(OpenAL REQUIRED)

include_directories(
        ${OPENAL_INCLUDE_DIR}
        ${COFFEE_CORE_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set ( CoffeeAudio_SRCS
        ${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c
        private/caudio.cpp
        private/openal/copenal.cpp
        private/abstraction/openal/calsoundbuffer.cpp
        private/abstraction/openal/calsounddevice.cpp
        private/abstraction/openal/calsounddeviceidentifier.cpp
        private/abstraction/openal/calsoundmixer.cpp
        private/abstraction/openal/calsoundmanager.cpp
        private/abstraction/openal/calsoundformat.cpp
        private/abstraction/openal/calsoundstream.cpp
        private/abstraction/openal/calsoundtrack.cpp
        private/abstraction/openal/calsoundsample.cpp
        )

coffee_add_library ( CoffeeAudio "${CoffeeAudio_SRCS}" )

target_link_libraries ( CoffeeAudio
    CoffeeCore
    ${OPENAL_LIBRARY}
    )
