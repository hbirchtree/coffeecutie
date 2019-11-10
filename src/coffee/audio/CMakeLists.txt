option ( BUILD_OPENAL "Build with OpenAL audio backend" ON )

find_package( stb REQUIRED )

coffee_library (
    TARGET Audio
    SOURCES
    # This is the stb_vorbis decoder. Say hello, stb
    ${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c

    # This is a collection of general audio functions
    private/caudio.cpp

    LIBRARIES Core

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>

    )

target_include_directories ( Audio SYSTEM PRIVATE
    ${STB_INCLUDE_DIR}
    )

if(BUILD_OPENAL)
    if(NOT APPLE)
        dependency_get (
            REQUIRED
            SOURCE hbirchtree/native-library-bundle
            TAG ${NATIVE_LIB_BUNDLE_TAG}
            NAMES openal
            )
    endif()

    find_package( OpenAL REQUIRED )

    coffee_library (
        TARGET OpenALMix
        SOURCES
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

        LIBRARIES Audio CoreObject
        )

    if(NOT APPLE AND NOT WIN32)
        target_link_libraries ( OpenALMix
            PUBLIC
            OpenAL
            )
        dependency_link (
            TARGET OpenALMix
            DEPENDENCIES
                openal
            )
    elseif( WIN32 )
        target_link_libraries ( OpenALMix
            PUBLIC
            ${OPENAL_LIBRARY}
            )
    elseif( APPLE )
        target_link_libraries ( OpenALMix
            PUBLIC
            "-framework OpenAL"
            )
    endif()

    target_include_directories ( OpenALMix SYSTEM PRIVATE
        ${OPENAL_INCLUDE_DIR}
        )
endif()
