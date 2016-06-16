#From: https://github.com/cmccabe/lksmith/blob/master/cmake_modules/FindLibunwind.cmake

# Find the libunwind library
#
#  LIBUNWIND_FOUND       - True if libunwind was found.
#  LIBUNWIND_LIBRARIES   - The libraries needed to use libunwind
#  LIBUNWIND_INCLUDE_DIR - Location of unwind.h and libunwind.h

FIND_PATH(LIBUNWIND_INCLUDE_DIR libunwind.h)

if(NOT EXISTS "${LIBUNWIND_INCLUDE_DIR}/unwind.h")
  SET(LIBUNWIND_INCLUDE_DIR "")
endif()

FIND_LIBRARY(LIBUNWIND_GENERIC_LIBRARY "unwind")
SET(LIBUNWIND_LIBRARIES ${LIBUNWIND_GENERIC_LIBRARY})

# For some reason, we have to link to two libunwind shared object files:
# one arch-specific and one not.
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    SET(LIBUNWIND_ARCH "arm")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
    SET(LIBUNWIND_ARCH "x86_64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^i.86$")
    SET(LIBUNWIND_ARCH "x86")
endif()

if (LIBUNWIND_ARCH)
    FIND_LIBRARY(LIBUNWIND_SPECIFIC_LIBRARY "unwind-${LIBUNWIND_ARCH}")
    SET(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARIES} ${LIBUNWIND_SPECIFIC_LIBRARY})
endif(LIBUNWIND_ARCH)

if( LIBUNWIND_LIBRARIES AND LIBUNWIND_INCLUDE_DIR )
    set ( LIBUNWIND_FOUND )
endif()

MARK_AS_ADVANCED(LIBUNWIND_LIBRARIES LIBUNWIND_INCLUDE_DIR)
