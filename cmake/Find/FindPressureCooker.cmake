if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    set ( TOOL_ARCH "ubuntu.amd64" CACHE STRING "" )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set ( TOOL_ARCH "osx.x86_64" CACHE STRING "" )
else()
    set ( TOOL_ARCH "Unknown" CACHE STRING "" )
endif()

set ( PRESSURE_COOKER_LIB_DIR "${NATIVE_LIBRARY_ROOT}/Tools/${TOOL_ARCH}/lib" CACHE DIRECTORY "" )

find_program ( PRESSURE_COOKER_BIN
    NAMES PressureCooker

    PATHS
    ${NATIVE_LIBRARY_ROOT}/Tools/${TOOL_ARCH}

    PATH_SUFFIXES
    bin
    )

if(PRESSURE_COOKER_BIN AND PRESSURE_COOKER_LIB_DIR)
    macro ( PACKAGE_DIRECTORY TARGET VFS_SOURCE VFS_TARGET EXTENSIONS )
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD

            COMMAND
            ${CMAKE_COMMAND} -E env
            LD_LIBRARY_PATH=${PRESSURE_COOKER_LIB_DIR}
            DYLD_LIBRARY_PATH=${PRESSURE_COOKER_LIB_DIR}
            ${PRESSURE_COOKER_BIN}
                -e ${EXTENSIONS}
                ${VFS_SOURCE}
                ${VFS_TARGET}
            )
    endmacro()
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    PressureCooker
    REQUIRED_VARS
    PRESSURE_COOKER_BIN
    PRESSURE_COOKER_LIB_DIR
    )
