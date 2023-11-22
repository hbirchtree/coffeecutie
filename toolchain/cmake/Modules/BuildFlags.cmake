# ##############################################################################
# Static builds by default
# ##############################################################################

set(COFFEE_LINK_OPT STATIC)

# ##############################################################################
# Embedding a build string
# ##############################################################################

if(NOT DEFINED COFFEE_BUILD_STRING)
  # Build time strings, embedded within constexpr strings to keep track of when
  # a build was made. Because file timestamps are unreliable.
  string(TIMESTAMP BUILDTIME "%y%m%d%H")

  # git hash is retrieved
  execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_HASH
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

  # git hash to tag mapping
  execute_process(
    COMMAND git describe --tags ${GIT_HASH}
    OUTPUT_VARIABLE GIT_TAG
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

  string(SUBSTRING "${GIT_HASH}" "" 10 GIT_HASH)

  set(COFFEE_BUILD_STRING
      "${GIT_TAG}-${GIT_HASH}-${BUILDTIME}"
      CACHE STRING ""
  )
endif()

# ##############################################################################
# Disabling dynamic linking on unsupported platforms
# ##############################################################################

if((APPLE AND IOS)
   OR GAMECUBE
   OR WIN32
)
  set(NO_DYNAMIC_LINKING ON)
else()
  set(NO_DYNAMIC_LINKING OFF)
endif()
