set ( CoffeeCore_SRCS

    private/extern_storage.cpp
#    private/exit_handling.cpp
    private/argument_handling.cpp

    # General functions
    private/coffee.cpp
    private/platform_data.cpp

    # Internal object / debug functions
    private/base/cdebug.cpp
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

    # Compression / data storage
    private/datastorage/compression/libz.cpp
    private/datastorage/text/ini/ciniparser.cpp

    # Unit testing
    private/unit_tests/framework.cpp

    # We slap tinyXML2 in here, just cause
    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
    )

# Platform-specific code, put it here

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
    )
endif()

# Linux-specific code
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/linux/sysinfo.cpp
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

        )
endif()

# POSIX code
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/posix/environment.cpp
        )
endif()

# Some variables
set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

if(COFFEE_BUILD_OPENSSL)
    # OpenSSL used for ASIO networking
    find_package(OpenSSL REQUIRED)
    list ( APPEND CORE_INCLUDE_DIR
        ${OPENSSL_INCLUDE_DIR}
        )
    list ( APPEND CORE_EXTRA_LIBRARIES ${OPENSSL_LIBRARIES} )
endif()

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
    find_package ( X11 REQUIRED )
    list ( APPEND CORE_EXTRA_LIBRARIES ${X11_LIBRARIES} )
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
    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
endif()

if(ANDROID)
    # Add the Android logging library, as well as android and GLESv*
    list ( APPEND CORE_EXTRA_LIBRARIES
        # Shim functions for JNI
        AndroidCore
        # Logging and Android functions
        log android
        # OpenGL ES
        # TODO: Make GLES3 optional
        GLESv1_CM GLESv2 GLESv3
        )
endif()

if(WIN32)
    # Don't know what this is, but it's necessary
    list ( APPEND CORE_EXTRA_LIBRARIES pathcch )
endif()

coffee_add_library ( CoffeeCore "${CoffeeCore_SRCS}" )
include_directories ( SYSTEM ${CORE_INCLUDE_DIR} )
target_link_libraries ( CoffeeCore ${CORE_EXTRA_LIBRARIES} )
