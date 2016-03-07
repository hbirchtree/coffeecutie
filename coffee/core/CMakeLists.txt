set ( CoffeeCore_SRCS

    private/coffee.cpp
    private/base/cdebug.cpp
    private/base/cdisplay.cpp
    private/base/cfiles.cpp
    private/base/cfilewatcher.cpp
    private/base/cobject.cpp

    private/base/cdrendererbase.cpp

    private/functional/cqueuerunner.cpp

    private/profiler/profiling-export.cpp

    private/unit_tests/data_types.cpp
    private/unit_tests/micro_tests.cpp

    private/input/cinputfunctions.cpp

    private/plat/environment_details.cpp
    private/plat/plat_file.cpp
    private/plat/linking/libraries.cpp
    private/plat/windows/win_core.cpp
    private/plat/unix/unix_core.cpp

    # We slap tinyXML2 in here, just cause
    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
    )

set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

# OpenSSL used for ASIO networking
find_package(OpenSSL REQUIRED)
list ( APPEND CORE_INCLUDE_DIR
    ${OPENSSL_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeCore "${CoffeeCore_SRCS}" )

list ( APPEND CORE_EXTRA_LIBRARIES ${OPENSSL_LIBRARIES} )

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

if(ANDROID AND SDL2_FOUND)
    # We use SDL2 for some platform functionality on Android, like file storage
    find_package(SDL2 REQUIRED)
    list ( APPEND CORE_INCLUDE_DIR
        ${SDL2_INCLUDE_DIR}
        )
    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
endif()

if(WIN32)
    # Don't know what this is, but it's necessary
    list ( APPEND CORE_EXTRA_LIBRARIES pathcch )
endif()

include_directories ( ${CORE_INCLUDE_DIR} )
target_link_libraries ( CoffeeCore ${CORE_EXTRA_LIBRARIES} )