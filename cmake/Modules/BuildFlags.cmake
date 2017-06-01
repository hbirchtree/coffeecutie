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

# Enable unit testing with GENERATE_TESTS
if(GENERATE_TESTS)
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

if(COFFEE_BUILD_ANGLE)
	add_definitions( "-DCOFFEE_GLES20_MODE" )
endif()

# Windows is missing an identifier for libraries
if(WIN32)
    if(${CMAKE_CXX_COMPILER} MATCHES "/x86_amd64/")
        set ( CMAKE_LIBRARY_ARCHITECTURE_ "amd64" )
		set ( CMAKE_LIBRARY_ARCHITECTURE_UWP_ "x64" )
		set ( CMAKE_LIBRARY_ARCHITECTURE_SDL_ "x64" )
    elseif(${CMAKE_CXX_COMPILER} MATCHES "/x86_arm/")
        set ( CMAKE_LIBRARY_ARCHITECTURE_ "arm" )
		set ( CMAKE_LIBRARY_ARCHITECTURE_UWP_ "arm" )
    else()
        set ( CMAKE_LIBRARY_ARCHITECTURE_ "x86" )
		set ( CMAKE_LIBRARY_ARCHITECTURE_UWP_ "Win32" )
		set ( CMAKE_LIBRARY_ARCHITECTURE_SDL_ "x86" )
    endif()

    set ( CMAKE_LIBRARY_ARCHITECTURE "${CMAKE_LIBRARY_ARCHITECTURE_}" CACHE STRING "" )
	set ( CMAKE_LIBRARY_ARCHITECTURE_UWP "${CMAKE_LIBRARY_ARCHITECTURE_UWP_}" CACHE STRING "" )
	set ( CMAKE_LIBRARY_ARCHITECTURE_SDL "${CMAKE_LIBRARY_ARCHITECTURE_SDL_}" CACHE STRING "" )
endif()
