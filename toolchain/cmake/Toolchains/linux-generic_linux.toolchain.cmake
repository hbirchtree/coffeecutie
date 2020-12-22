set ( TOOLCHAIN_ROOT "/usr" CACHE PATH "" )
set ( TOOLCHAIN_PREFIX "" CACHE STRING "" )

if ( "${TOOLCHAIN_PREFIX}" STREQUAL "" )
    set ( TOOLCHAIN_TOOL "" CACHE STRING "" )
else()
    set ( TOOLCHAIN_TOOL "${TOOLCHAIN_PREFIX}-" CACHE STRING "" )
endif()

set (CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set (CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

add_definitions (-fvisibility=hidden)

if(LINUX_PARANOID_BUILD)
    add_definitions (
        -Winline
        -Wall
        -Werror
        -Wpadded
        )

    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions" )
endif()
