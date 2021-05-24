set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR x86_64 ) 

set ( CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE FILEPATH "" )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake )

set ( BR2_SYSROOT ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/sysroot )

set ( CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc" CACHE FILEPATH "" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++" CACHE FILEPATH "" )

set ( SHARED_LINK_FLAGS
    # ${CMAKE_SYSROOT}/usr/lib/libc.a
    #"-static-libstdc++"
    #"-static-libstdc++ -Wl,-rpath-link,${BR2_SYSROOT}/usr/lib"
    #"-Wl,-rpath-link,${BR2_SYSROOT}/usr/lib"
    )

set ( CMAKE_FIND_ROOT_PATH
    ${BR2_SYSROOT}
    ${BR2_SYSROOT}/usr
    )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake )

set ( CMAKE_IGNORE_PATH /usr /usr/local )

add_definitions (-fvisibility=hidden)
