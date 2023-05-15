if("${TOOLCHAIN_ROOT}" STREQUAL "")
  if(NOT "$ENV{TOOLCHAIN_ROOT}" STREQUAL "")
    set(TOOLCHAIN_ROOT
        "$ENV{TOOLCHAIN_ROOT}"
        CACHE PATH ""
    )
    set(TOOLCHAIN_PREFIX
        "$ENV{TOOLCHAIN_PREFIX}"
        CACHE STRING ""
    )
  else()
    message(ERROR "Compiler not found")
  endif()
else()
  set(TOOLCHAIN_ROOT
      "${TOOLCHAIN_ROOT}"
      CACHE PATH ""
  )
  set(TOOLCHAIN_PREFIX
      "${TOOLCHAIN_PREFIX}"
      CACHE STRING ""
  )
endif()

if("${TOOLCHAIN_PREFIX}" STREQUAL "")
  set(TOOLCHAIN_TOOL "")
else()
  set(TOOLCHAIN_TOOL "${TOOLCHAIN_PREFIX}-")
endif()

set(CMAKE_SYSROOT
    ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/sysroot
    CACHE PATH ""
)
