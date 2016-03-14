# Fiddle with warning flags
if (NOT WIN32)
    # These are effectively used by Clang and GCC, descriptive names
    add_definitions (
         -Winline
#        -Wall
#        -Werror
#        -Wpadded
        )
else()
    # For Windows, we disable most annoying warnings to save compilation time.
    # These include system headers (which are constantly giving off warnings)
    #  as well as small warnings for padding and etc.
    # Microsoft makes the worst headers of them all.
    add_definitions (
        -W4
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
add_definitions(-D__CBUILDTIME__="${CBUILDTIME}")

# The below will cause a complete rebuild of the library at every compilation, *from the bottom*.
# Be wary of this. In most cases you would do this for relatively long-term builds
#add_custom_target(invalidate_files ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/coffee/core/coffee_macros.h)
