################################################################################
# Static builds by default
################################################################################

set(COFFEE_LINK_OPT STATIC)

################################################################################
# Embedding a build string
################################################################################

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

################################################################################
# Disabling dynamic linking on unsupported platforms
################################################################################

if( (APPLE AND IOS) OR GAMECUBE )
    set ( NO_DYNAMIC_LINKING ON )
else()
    set ( NO_DYNAMIC_LINKING OFF )
endif()
