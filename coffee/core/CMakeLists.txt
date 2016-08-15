set ( CoffeeCore_SRCS

    private/extern_storage.cpp
#    private/exit_handling.cpp
    private/argument_handling.cpp

    # General functions
    private/coffee.cpp
    private/platform_data.cpp

    # Internal object / debug functions
    private/base/cdebug.cpp
    private/base/cdisplay.cpp
    private/base/cfiles.cpp
    private/base/cobject.cpp

    # Terminal functions
    private/terminal/table-print.cpp

    # Profiling
    private/profiler/profiling-export.cpp

    # TODO: Move Windows library code into new API
    private/plat/linking/libraries.cpp

    # Isolation of platform libraries
    private/plat/environment_wall.cpp

    # Internal platform layer
    private/plat/environment/application_start.cpp
    private/plat/environment/argument_parse.cpp

    # Compression / data storage
    private/datastorage/compression/libz.cpp
    private/datastorage/text/ini/ciniparser.cpp

    # Unit testing
    private/unit_tests/framework.cpp

    # We slap tinyXML2 in here, just cause
    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
    )

# Platform-specific code, put it here

# EGL loading
if( ANDROID )
    list ( APPEND CoffeeCore_SRCS
#            private/plat/graphics/eglinit.cpp
            )
endif()

# SDL power-management
if( ANDROID_USE_SDL2_LAUNCH OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR WIN32)
	set ( SDL_POWER_PLUGIN_ENABLED ON )
    list ( APPEND CoffeeCore_SRCS
	private/plat/environment/sdlpowerinfo.cpp
	)
endif()

# Android-specific code
if(ANDROID)
    list ( APPEND CoffeeCore_SRCS
        ${CMAKE_SOURCE_DIR}/bindings/android/static.cpp
        private/plat/file/android/file.cpp
        private/plat/environment/android/environment.cpp
        private/plat/environment/android/sysinfo.cpp
        private/plat/sensor/android/sensor.cpp
    )
endif()

# Linux-specific code
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/linux/sysinfo.cpp
        private/plat/file/linux/file.cpp
	private/plat/environment/linux/environment.cpp
        )
endif()

# Mac OS X-specific code
if(APPLE)
    list ( APPEND CoffeeCore_SRCS
        private/plat/file/osx/file.cpp
        )
endif()

# Windows-specific code
if(WIN32)
    list ( APPEND CoffeeCore_SRCS
        private/plat/file/windows/file.cpp
        private/plat/environment/windows/environment.cpp
        private/plat/environment/windows/sysinfo.cpp
        )
endif()

# POSIX code
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/unix/environment.cpp
        private/plat/file/unix/file.cpp
        private/plat/linking/unix/loader.cpp
        )
endif()

# Some variables
set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

# Platform-specific target options

if(NOT WIN32 AND NOT MINGW AND NOT MSYS)
    # Necessary for Linux and possibly OS X (latter is untested)
    list ( APPEND CORE_EXTRA_LIBRARIES dl m )
endif()

if(NOT WIN32 AND NOT ANDROID)
    # Used for thread details
    list ( APPEND CORE_EXTRA_LIBRARIES pthread )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this library
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    find_package(Unwind QUIET )
    if (LIBUNWIND_FOUND)
        add_definitions ( -DCOFFEE_USE_UNWIND )
        list ( APPEND CORE_INCLUDE_DIR
            ${LIBUNWIND_INCLUDE_DIR}
            )
        list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
    endif()
endif()

if( SDL_POWER_PLUGIN_ENABLED OR ANDROID )
    # We use SDL2 for some platform functionality, like power info
    # On Android, it is also used to read assets and
    #  acquiring device info.
    find_package(SDL2 REQUIRED)
    list ( APPEND CORE_INCLUDE_DIR
        ${SDL2_INCLUDE_DIR}
        )
#    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
endif()

if(ANDROID)
    # Add the Android logging library, as well as android and GLESv*
    list ( APPEND CORE_EXTRA_LIBRARIES
        # Shim functions for JNI
        AndroidCore
        # Logging and Android functions
        log android
        # OpenGL ES
        GLESv1_CM GLESv2
        )
    if("${ANDROID_NATIVE_API_LEVEL}" GREATER 17)
        message ( "-- Building with GLES 3.0+ support" )
        list ( APPEND CORE_EXTRA_LIBRARIES
                GLESv3
                )
    endif()
endif()

if(WIN32)
    # Don't know what this is, but it's necessary
    list ( APPEND CORE_EXTRA_LIBRARIES
	 # For some of the file API
	 pathcch

	 # Core includes
	 user32
	 gdi32
	 winmm
	 imm32
	 ole32
	 oleaut32
	 shell32
	 version
	  )
endif()

coffee_add_library ( CoffeeCore "${CoffeeCore_SRCS}" )
include_directories ( SYSTEM ${CORE_INCLUDE_DIR} )
target_link_libraries ( CoffeeCore ${CORE_EXTRA_LIBRARIES} )
