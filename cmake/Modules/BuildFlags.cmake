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

# The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
# Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

# RapidJSON is part of the core, and is therefore added as a submodule
find_package ( RapidJson REQUIRED )
include_directories ( ${RAPIDJSON_INCLUDE_DIR} )

# This causes ASIO to not use Boost.
add_definitions("-DASIO_STANDALONE")

# Include SSL if built
if(COFFEE_BUILD_OPENSSL AND NOT WIN32 AND NOT ANDROID)
    find_package ( OpenSSL REQUIRED )
    include_directories ( ${OPENSSL_INCLUDE_DIR} )
endif()

if(ANDROID AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions("-DANDROID_RELEASE_MODE")
endif()

# Enable testing with COFFEE_TESTS
if(COFFEE_TESTS)
    enable_testing()
endif()

# Android requires SDL for asset functionality
# GLES uses SDL for function loading
if(ANDROID)
#    if(ANDROID_USE_SDL2_LAUNCH)
        message("-- Building Android project with SDL2 bindings")
#    else()
#        message("-- Building Android project with native_app_glue bindings")
#        add_definitions("-DANDROID_DONT_USE_SDL2")
#    endif()

    add_definitions ( "-DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}" )
endif()

if(NOT COFFEE_BUILD_GLES)
    add_definitions("-DCOFFEE_GLEAM_DESKTOP")
endif()
