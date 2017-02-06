set ( CMAKE_SYSTEM_NAME Windows )
set ( TOOLCHAIN_PREFIX "x86_64-w64-mingw32" )

set ( MINGW64 ON CACHE BOOL "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )
set ( CMAKE_RC_COMPILER "${TOOLCHAIN_PREFIX}-windres" )

add_definitions (
    -DMINGW64
)

set ( CMAKE_FIND_ROOT_PATH "/usr/${TOOLCHAIN_PREFIX};/usr/lib/gcc/${TOOLCHAIN_PREFIX}/5.3-posix" )

set ( CMAKE_FIND_ROO_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

include_directories ( BEFORE "/usr/lib/gcc/${TOOLCHAIN_PREFIX}/5.3-win32/include/c++" )
link_directories ( "/usr/lib/gcc/${TOOLCHAIN_PREFIX}/5.3-posix" )
