# Static builds
set(COFFEE_LINK_OPT STATIC)

# Build time strings, embedded within constexpr strings to keep track of when a build was made.
# Because file timestamps are unreliable.
string (TIMESTAMP CBUILDTIME "%y.%m.%d.%H%M")

set ( COFFEE_VERSION_CODE "2.1.1" )

# git hash is retrieved
execute_process (
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_HASH
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

string ( SUBSTRING "${GIT_HASH}" "" 10 GIT_HASH )

#set ( GIT_HASH "00000000" )

set ( COFFEE_BUILD_STRING "${COFFEE_VERSION_CODE}.${CBUILDTIME}-${GIT_HASH}" )

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
