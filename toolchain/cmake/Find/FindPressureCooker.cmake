find_program ( PRESSURE_COOKER_BIN
    NAMES PressureCooker

    PATHS
    ${PRESSURECOOKER_DIR}

    PATH_SUFFIXES
    bin
    bin/Release
    bin/Debug
    )

if(PRESSURE_COOKER_BIN)
    macro ( PACKAGE_DIRECTORY TARGET VFS_SOURCE VFS_TARGET )

        set ( PRESSURE_COOKER_LIB_DIR
            ${PRESSURECOOKER_EXTRA_LIBS_DIR}
            ${PRESSURECOOKER_DIR}/lib
            ${PRESSURECOOKER_DIR}/lib/Debug
            ${PRESSURECOOKER_DIR}/lib/Release
            )

        string (
            REPLACE ";" ":"
            PRESSURE_COOKER_LIB_DIR "${PRESSURE_COOKER_LIB_DIR}"
            )

        set ( PACKAGE_DIRECTORY_IGNORE_TYPES "blend,blend1" )
        set ( PACKAGE_DIRECTORY_COMPRESS_TYPES "fbx,blend,blend1" )

        cmake_parse_arguments ( PACKAGE_DIRECTORY
            ""
            "EXTENSIONS;IGNORE_TYPES;BASE_DIRS;COMPRESS_TYPES;CACHE_DIR"
            "ENV_VARS"
            ${ARGN}
            )

        get_filename_component ( OUTPUT_DIR "${VFS_TARGET}" DIRECTORY )

        set ( PACKAGING_ARGS "" )

        if(DEFINED PACKAGE_DIRECTORY_EXTENSIONS)
            set ( PACKAGING_ARGS
                ${PACKAGING_ARGS} -e ${PACKAGE_DIRECTORY_EXTENSIONS} )
        endif()

        if(DEFINED PACKAGE_DIRECTORY_IGNORE_TYPES)
            set ( PACKAGING_ARGS
                ${PACKAGING_ARGS} -i ${PACKAGE_DIRECTORY_IGNORE_TYPES} )
        endif()

        if(DEFINED PACKAGE_DIRECTORY_COMPRESS_TYPES)
            set ( PACKAGING_ARGS
                ${PACKAGING_ARGS} -c ${PACKAGE_DIRECTORY_COMPRESS_TYPES} )
        endif()

        if(DEFINED PACKAGE_DIRECTORY_BASE_DIRS)
            set ( PACKAGING_ARGS
                ${PACKAGING_ARGS} -b ${PACKAGE_DIRECTORY_BASE_DIRS} )
        endif()

        if(DEFINED PACKAGE_DIRECTORY_CACHE_DIR)
            set ( PACKAGING_ARGS
                ${PACKAGING_ARGS} -m ${PACKAGE_DIRECTORY_CACHE_DIR} )
        endif()

        if(DEFINED PACKAGE_DIRECTORY_ENV_VARS)
            set ( ENV_VARS ${PACKAGE_DIRECTORY_ENV_VARS} )
        endif()

        file ( GLOB_RECURSE VFS_DEPENDS
            "${VFS_SOURCE}/*"
            )

        add_custom_command ( OUTPUT ${VFS_TARGET}

            COMMAND
            ${CMAKE_COMMAND} -E env
            LD_LIBRARY_PATH=${PRESSURE_COOKER_LIB_DIR}
            DYLD_LIBRARY_PATH=${PRESSURE_COOKER_LIB_DIR}
            ${ENV_VARS}
            ${PRESSURE_COOKER_BIN}
                ${PACKAGING_ARGS}
                ${VFS_SOURCE}
                ${VFS_TARGET}

            DEPENDS ${VFS_DEPENDS}
            )

        add_custom_target ( ${TARGET}
            COMMAND
            ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}

            DEPENDS ${VFS_TARGET}
            )

    endmacro()
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    PressureCooker
    REQUIRED_VARS
    PRESSURE_COOKER_BIN
    )
