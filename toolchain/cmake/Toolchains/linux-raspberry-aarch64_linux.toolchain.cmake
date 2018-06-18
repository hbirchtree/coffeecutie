set ( CMAKE_SYSTEM_NAME Linux )
set ( TOOLCHAIN_PREFIX "aarch64-linux-gnu" )

set ( RASPBERRY TRUE )

set ( RASPBERRY_SDK "${RASPBERRY_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

set ( CMAKE_FIND_ROOT_PATH 
    ${RASPBERRY_SDK}
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/src/libs
    )
set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

include_directories ( ${RASPBERRY_SDK}/vc/include ${RASPBERRY_SDK}/usr/include )
link_directories ( ${RASPBERRY_SDK}/vc/lib )

add_definitions(-D__RASPBERRYPI__)
add_definitions(-D__RASPBERRYPI64__)
