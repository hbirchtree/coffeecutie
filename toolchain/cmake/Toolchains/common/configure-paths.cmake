set(CMAKE_C_FLAGS
    "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS}"
    CACHE STRING ""
)
set(CMAKE_CXX_FLAGS
    "${CMAKE_C_FLAGS} ${SHARED_FLAGS}"
    CACHE STRING ""
)

set(CMAKE_EXE_LINKER_FLAGS
    "${SHARED_FLAGS} ${SHARED_LINK_FLAGS}"
    CACHE STRING ""
)
set(CMAKE_MODULE_LINKER_FLAGS
    "${SHARED_LINK_FLAGS}"
    CACHE STRING ""
)
set(CMAKE_STATIC_LINKER_FLAGS
    "${SHARED_LINK_FLAGS}"
    CACHE STRING ""
)
set(CMAKE_SHARED_LINKER_FLAGS
    "${SHARED_LINK_FLAGS}"
    CACHE STRING ""
)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT} ${CMAKE_SYSROOT}/usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_IGNORE_PATH /usr /usr/local)

# set ( ENV{PKG_CONFIG_DIR} "" )
set(ENV{PKG_CONFIG_PATH} ${CMAKE_SYSROOT}/usr/lib/pkgconfig)
set(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_SYSROOT}/usr/lib/pkgconfig)
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
set(CMAKE_LIBRARY_ARCHITECTURE
    "${TOOLCHAIN_PREFIX}"
    CACHE STRING ""
)
set(PKG_CONFIG_PATH ${CMAKE_SYSROOT}/usr/lib/pkgconfig)

foreach(PATH ${CMAKE_FIND_ROOT_PATH})
  include_directories(SYSTEM ${PATH}/include)
  link_directories(${PATH}/lib)
  set(PKG_CONFIG_PATH ${PKG_CONFIG_PATH}:${PATH}/lib/pkgconfig)
endforeach()

set(CMAKE_INSTALL_RPATH
    "../lib"
    CACHE STRING ""
)

# set ( CMAKE_LIBRARY_PATH ${CMAKE_SYSROOT}/lib ${CMAKE_SYSROOT}/usr/lib )
