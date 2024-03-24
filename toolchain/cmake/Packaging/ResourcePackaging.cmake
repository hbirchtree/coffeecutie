find_program ( GLSLANG_PROGRAM glslangValidator )
find_program ( INKSCAPE_PROGRAM inkscape
    PATHS
        /usr
        /usr/local
    PATH_SUFFIXES
        bin
    )
set(SHADER_COOKER_PROGRAM "${HOST_TOOLS_BINARY_DIR}/ShaderCooker" CACHE FILEPATH "")
set(TEXTURE_COMPRESSOR_PROGRAM "${HOST_TOOLS_BINARY_DIR}/TextureCompressor" CACHE FILEPATH "")

function(RESOURCE_DIR_PACKAGE)
    set(SINGLE_OPTS
        TARGET
        SOURCE_DIR
        OUT_DIR
        CACHE_DIR
    )

    set(MULTI_OPTS
        EXTRA_ARGS
    )

    cmake_parse_arguments(RPKG "" "${SINGLE_OPTS}" "${MULTI_OPTS}" ${ARGN})

    if("${RPKG_TARGET}" STREQUAL "")
        message(ERROR "No target specified for RESOURCE_DIR_PACKAGE")
    endif()
    if("${RPKG_OUT_DIR}" STREQUAL "")
        set(RPKG_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
    endif()
    if("${RPKG_CACHE_DIR}" STREQUAL "")
        set(RPKG_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/asset_cache")
    endif()
    if("${RPKG_SOURCE_DIR}" STREQUAL "")
        message(ERROR "No source dir specified for RESOURCE_DIR_PACKAGE of ${RPKG_TARGET}")
    endif()

    set(ARG_LIST
        #
        --path ${RPKG_SOURCE_DIR}
        #
        --output ${RPKG_OUT_DIR}
        #
        --cache ${RPKG_CACHE_DIR}
        #
        --program ShaderCooker=${SHADER_COOKER_PROGRAM}
        #
        --program Inkscape=${INKSCAPE_PROGRAM}
        #
        --program glslang=${GLSLANG_PROGRAM}
        #
        --program TextureCompressor=${TEXTURE_COMPRESSOR_PROGRAM}
        #
        --program glslang=${GLSLANG_PROGRAM}
        #
        --target ${CMAKE_SYSTEM_NAME}
        #
        --build-mode Release
        #
        ${RPKG_EXTRA_ARGS}
    )

    add_custom_target (
      ${RPKG_TARGET}
      COMMAND
        ${CMAKE_SOURCE_DIR}/toolchain/desktop/resource_compile.py ${ARG_LIST}
    )

    if(WIN32 OR APPLE)
        # When we're using GLOB_RECURSE, we need the files to exist
        # before configuring the target
        execute_process(COMMAND
            ${CMAKE_SOURCE_DIR}/toolchain/desktop/resource_compile.py ${ARG_LIST}
        )
    endif()
endfunction()
