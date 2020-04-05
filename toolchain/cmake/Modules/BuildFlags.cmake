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
        COMMAND ${CMAKE_SOURCE_DIR}/toolchain/version.py none --version
        OUTPUT_VARIABLE COFFEE_VERSION_CODE
        )

    string ( REGEX REPLACE "\n$" "" COFFEE_VERSION_CODE "${COFFEE_VERSION_CODE}" )

    set ( COFFEE_VERSION_CODE "${COFFEE_VERSION_CODE}"
        CACHE STRING "" )


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


################################################################################
# Disabling dynamic linking on unsupported platforms
################################################################################

if( (APPLE AND IOS) OR GAMECUBE )
    set ( NO_DYNAMIC_LINKING ON )
else()
    set ( NO_DYNAMIC_LINKING OFF )
endif()
