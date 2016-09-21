set ( CoffeeCore_SRCS

    private/extern_storage.cpp
#    private/exit_handling.cpp
    private/coffee_signals.cpp
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

if(RASPBERRY)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/raspberry/sysinfo.cpp
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

find_package( StandardLibs REQUIRED )

coffee_add_library ( CoffeeCore "${CoffeeCore_SRCS}" )
include_directories ( SYSTEM ${STANDARDLIBS_INCLUDE_DIR} )
target_link_libraries ( CoffeeCore ${STANDARDLIBS_LIBRARIES} )
