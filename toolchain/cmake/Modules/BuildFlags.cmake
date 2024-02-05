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
  string(TIMESTAMP BUILDTIME "%y%m%dT%H")

  # summarize git tag, commit and working tree
  execute_process(
    COMMAND git describe --tags --dirty
    OUTPUT_VARIABLE GIT_TAG
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(COFFEE_BUILD_STRING
      "${GIT_TAG}-${BUILDTIME}"
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
