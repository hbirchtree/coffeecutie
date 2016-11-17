find_package(OpenAL REQUIRED)

set ( CoffeeAudio_SRCS
	# This is the stb_vorbis decoder. Say hello, stb
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

include_directories(${OPENAL_INCLUDE_DIR})

coffee_add_library ( CoffeeOpenAL "${CoffeeAudio_SRCS}" "CoffeeCore;${OPENAL_LIBRARY}" )
