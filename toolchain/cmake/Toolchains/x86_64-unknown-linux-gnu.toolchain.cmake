set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR x86_64 )

set ( CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE FILEPATH "" )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake )

set ( CMAKE_SYSROOT ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/sysroot )

set ( CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc" CACHE FILEPATH "" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++" CACHE FILEPATH "" )

add_link_options (
    -static-libstdc++
    -static-libgcc
    -lssp_nonshared
    -Wl,-rpath-link,../lib
    )

#set ( SHARED_LINK_FLAGS
##    "${CMAKE_SYSROOT}/lib/libstdc++.a"
##    "-static-libssp"
##    "-static-libgcc"
#    "-static-libstdc++ -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib"
#    "-static-libstdc++"
#    )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake )

set ( CMAKE_IGNORE_PATH /usr /usr/local )

add_definitions ( -fvisibility=hidden )
