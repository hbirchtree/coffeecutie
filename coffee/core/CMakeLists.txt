set ( CoffeeCore_SRCS

    private/extern_storage.cpp
#    private/exit_handling.cpp
    private/argument_handling.cpp


    private/coffee.cpp
    private/base/cdebug.cpp
    private/base/cfiles.cpp
    private/base/cobject.cpp

    private/profiler/profiling-export.cpp

    private/plat/linking/libraries.cpp

    private/plat/file/android/file.cpp
    private/plat/environment/android/environment.cpp

    private/plat/environment/application_start.cpp

    private/datastorage/compression/libz.cpp

    # We slap tinyXML2 in here, just cause
    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
    )

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
    find_package(X11 REQUIRED)
    list ( APPEND CORE_EXTRA_LIBRARIES ${X11_LIBRARIES} )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    find_package(Unwind QUIET)
    list ( APPEND CORE_INCLUDE_DIR
        ${LIBUNWIND_INCLUDE_DIR}
        )
    list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
endif()

if(ANDROID)
    # We use SDL2 for some platform functionality on Android, like file storage
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
