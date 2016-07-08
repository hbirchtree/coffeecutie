# Fiddle with warning flags
if (NOT WIN32)
    # These are effectively used by Clang and GCC, descriptive names
    add_definitions (
        -fvisibility=hidden
        #-Winline
        #-Wall
        #-Werror
        #-Wpadded
        )
    if(APPLE)
        include_directories ( "/usr/local/opt/llvm38/lib/llvm-3.8/include/c++/v1" )
	link_directories("/usr/local/opt/llvm38/lib/llvm-3.8/lib")
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
        set ( CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS}" )
    endif()
else()
    # For Windows, we disable most annoying warnings to save compilation time.
    # These include system headers (which are constantly giving off warnings)
    #  as well as small warnings for padding and etc.
    # Microsoft makes the worst headers of them all.
    # We also set a target Windows NT version for some reason
    add_definitions (
	-W1	
	-D_WIN32_WINNT=0x0601
)
endif()

# Static builds
if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

# Build time strings, embedded within constexpr strings to keep track of when a build was made.
# Because file timestamps are unreliable.
string (TIMESTAMP CBUILDTIME "%y.%m.%d.%H%M")

set ( COFFEE_VERSION_CODE "001" )

#add_definitions( -DCOFFEE_APPLICATION_NAME="Generic Coffee" )
add_definitions( -DCOFFEE_ORGANIZATION_NAME="Coffee Guy" )
add_definitions( -DCOFFEE_VERSION_CODE=${COFFEE_VERSION_CODE} )

# git hash is retrieved
execute_process (
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_HASH
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

string ( SUBSTRING "${GIT_HASH}" "" 10 GIT_HASH )

#set ( GIT_HASH "00000000" )

set ( COFFEE_BUILD_STRING "${COFFEE_VERSION_CODE}.00.00.${CBUILDTIME}-${GIT_HASH}" )

add_definitions( -DCOFFEE_BUILD_STRING="${COFFEE_BUILD_STRING}" )

# The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
# Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)

# RapidJSON is part of the core, and is therefore added as a submodule
find_package ( RapidJson REQUIRED )
include_directories ( ${RAPIDJSON_INCLUDE_DIR} )

# This causes ASIO to not use Boost.
add_definitions("-DASIO_STANDALONE")

# Toggle some preprocessor flags
if(COFFEE_BUILD_OCULUSVR)
    add_definitions("-DCOFFEE_OCULUSVR_ENABLED")
endif()

# Include SSL if built
if(COFFEE_BUILD_OPENSSL)
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
    if(ANDROID_USE_SDL2_LAUNCH)
        message("-- Building Android project with SDL2 bindings")
        find_package(SDL2 REQUIRED)
        include_directories(
            ${SDL2_INCLUDE_DIR}
            ${SDL2_INCLUDE_DIR}/SDL2)
    else()
        message("-- Building Android project with NDK bindings")
        add_definitions("-DANDROID_DONT_USE_SDL2")
        find_package(AndroidGlue)
        include_directories (${ANDROID_GLUE_INCLUDE_DIR})
    endif()
endif()

if(NOT COFFEE_BUILD_GLES)
    add_definitions("-DCOFFEE_GLEAM_DESKTOP")
else()

endif()
