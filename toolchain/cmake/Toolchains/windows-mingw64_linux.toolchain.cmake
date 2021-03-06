set ( CMAKE_SYSTEM_NAME Windows )
set ( TOOLCHAIN_PREFIX "x86_64-w64-mingw32" )

set ( MINGW64 ON CACHE BOOL "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc-posix" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++-posix" )
set ( CMAKE_RC_COMPILER "${TOOLCHAIN_PREFIX}-windres" )

add_definitions (
    -D__MINGW64__
    -D_WIN32_WINNT=0x0602
)

set ( CMAKE_FIND_ROOT_PATH
    /usr/${TOOLCHAIN_PREFIX}
    ${NATIVE_LIBRARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/libs
    ${COFFEE_ROOT_DIR}
    )

set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

add_compile_options ( -mwindows )

set ( CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" )
set ( CMAKE_LIBRARY_ARCHITECTURE "${TOOLCHAIN_PREFIX}" CACHE STRING "" )
