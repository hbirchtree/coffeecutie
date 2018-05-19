find_package(OpenAL REQUIRED)
find_package(stb REQUIRED)

set ( CoffeeAudio_SRCS
	# This is the stb_vorbis decoder. Say hello, stb
	${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c

    # This is a collection of general audio functions
	private/caudio.cpp
	)

set ( CoffeeOpenAL_SRCS
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

coffee_library (
    TARGET CoffeeAudio
    SOURCES ${CoffeeAudio_SRCS}
    LIBRARIES CoffeeCore
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/audio
    )

target_include_directories ( CoffeeAudio
    SYSTEM
    PRIVATE
    ${STB_INCLUDE_DIR}
    )

coffee_library (
    TARGET CoffeeOpenAL
    SOURCES ${CoffeeOpenAL_SRCS}
    LIBRARIES CoffeeAudio
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/audio/openal
    )

if(NOT APPLE)
    target_link_libraries ( CoffeeOpenAL
        PRIVATE
        OpenAL
        )
else()
    target_link_libraries ( CoffeeOpenAL
        PRIVATE
        "-framework OpenAL"
        )
endif()

target_include_directories ( CoffeeOpenAL
    SYSTEM
    PRIVATE
    ${OPENAL_INCLUDE_DIR}
    )
