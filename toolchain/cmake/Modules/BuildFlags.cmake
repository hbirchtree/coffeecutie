# Static builds
set(COFFEE_LINK_OPT STATIC)

if(NOT DEFINED COFFEE_BUILD_STRING)
    # Build time strings, embedded within constexpr strings to keep track of when a build was made.
    # Because file timestamps are unreliable.
    string (TIMESTAMP CBUILDTIME "%y%m%d%H")

    execute_process (
        COMMAND ${CMAKE_SOURCE_DIR}/version.py none --version
        OUTPUT_VARIABLE COFFEE_VERSION_CODE
        )

    string ( REGEX REPLACE "\n$" "" COFFEE_VERSION_CODE "${COFFEE_VERSION_CODE}" )

    set ( COFFEE_VERSION_CODE "${COFFEE_VERSION_CODE}"
        CACHE STRING "" )

    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT WIN32)
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wpedantic -Wno-gnu-anonymous-struct -Wno-nested-anon-types"
            CACHE STRING "" )
    endif()

    # git hash is retrieved
    execute_process (
        COMMAND git rev-parse HEAD
        OUTPUT_VARIABLE GIT_HASH
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

    string ( SUBSTRING "${GIT_HASH}" "" 10 GIT_HASH )

    set ( COFFEE_BUILD_STRING
        "${COFFEE_VERSION_CODE}.${CBUILDTIME}-${GIT_HASH}"
        CACHE STRING ""
        )

    file ( WRITE "${CMAKE_BINARY_DIR}/VERSION.${PROJECT_NAME}"
        "${COFFEE_VERSION_CODE}"
        )
endif()

install (
    FILES "${CMAKE_BINARY_DIR}/VERSION.${PROJECT_NAME}"
    DESTINATION share
    )

add_definitions( -DCOFFEE_BUILD_STRING="${COFFEE_BUILD_STRING}" )

if( (APPLE AND IOS) OR GAMECUBE )
    set ( COFFEE_NO_DYNAMIC_LINKING ON )
else()
    set ( COFFEE_NO_DYNAMIC_LINKING OFF )
endif()

# The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
# Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

# This causes ASIO to not use Boost.
add_definitions("-DASIO_STANDALONE")

# Include SSL if built
if(COFFEE_BUILD_OPENSSL)
    add_definitions("-DCOFFEE_ENABLE_SSL")
endif()

if(ANDROID AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions("-DANDROID_RELEASE_MODE")
endif()

# Enable unit testing with GENERATE_TESTS
if(GENERATE_TESTS)
    enable_testing()
endif()

# Android requires SDL for asset functionality
# GLES uses SDL for function loading
# This is only necessary for the core library
if(ANDROID)
    if(COFFEE_BUILD_SDL2)
        message(STATUS "Building Android project with SDL2 bindings")
    else()
        message(STATUS "Building Android project with NDK bindings")
    endif()

    add_definitions ( "-DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}" )
endif()

if(COFFEE_BUILD_ANGLE)
	add_definitions( "-DCOFFEE_GLES20_MODE" )
endif()

if(COFFEE_LOWFAT)
    add_definitions("-DCOFFEE_LOWFAT")
endif()

if(COFFEE_BUILD_SDL2)
    add_definitions("-DCOFFEE_USE_SDL2=1")
elseif(ANDROID)
    # Will be running on  Android NDK bindings
elseif(APPLE)
    # It will just use GLKit
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT EMSCRIPTEN AND NOT NACL AND NOT ANDROID)
    add_definitions( -DCOFFEE_LINUX_LIGHTWEIGHT_WM )
endif()

include ( WindowsPlatformDetect )

# Enabling ASan/TSan/UBSan
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(COFFEE_ASAN OR COFFEE_TSAN OR COFFEE_UBSAN)
        if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            message ( WARNING "Sanitizers should run in Debug mode!" )
        endif()

        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer" )
    endif()

    if(BUILD_COVERAGE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set ( CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping" )
    endif()

    if(COFFEE_ASAN)
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" )
    endif()
    if(COFFEE_TSAN)
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread" )
    endif()
    if(COFFEE_UBSAN)
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined" )
    endif()

    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" )
endif()