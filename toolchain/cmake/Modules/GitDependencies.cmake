set ( GIT_DEP_DIR "${CMAKE_BINARY_DIR}/dependencies" CACHE PATH "" )
set ( GIT_DEP_BUILDVARIANT "" CACHE STRING "" )

function ( DEPENDENCY_GET )

    cmake_parse_arguments ( DEP
        "REQUIRED"
        "EXTENSION;SOURCE;TAG;CACHE_LOCATION;LIB_LOCATION"
        "NAMES"

        ${ARGN}
        )

    if("${DEP_CACHE_LOCATION}" STREQUAL "")
        set ( DEP_CACHE_LOCATION "${GIT_DEP_DIR}/cache" )
    endif()

    if("${DEP_LIB_LOCATION}" STREQUAL "")
        set ( DEP_LIB_LOCATION "${GIT_DEP_DIR}" )
    endif()

    if("${DEP_EXTENSION}" STREQUAL "")
        set ( DEP_EXTENSION "tar.gz" )
    endif()

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${DEP_CACHE_LOCATION}"
        )

    set ( BASE_URL
        "https://github.com/${DEP_SOURCE}/releases/download/${DEP_TAG}" )

    foreach ( NAME_COMBO ${DEP_NAMES} )
        string ( REPLACE "=" ";" NAME_LIST "${NAME_COMBO}" )

        list ( LENGTH NAME_LIST NAME_LEN )
        list ( GET NAME_LIST 0 NAME )
        if( NAME_LEN GREATER 1 )
            list ( GET NAME_LIST 1 DEP_ALIAS )
        else()
            set ( DEP_ALIAS "${NAME}" )
        endif()

        set ( LOCAL_DIR "${DEP_LIB_LOCATION}/${NAME}" )

        #
        # In order to avoid globbing up the CMAKE_PREFIX_PATH,
        #  we only append it when it's not there.
        # We also have to use FORCE in order to make this work properly.
        # This is probably very bad, but lets FIND_PACKAGE() and pkg-config work
        #
        if(NOT "${CMAKE_PREFIX_PATH}" MATCHES "${LOCAL_DIR}" )
            set (
                CMAKE_PREFIX_PATH
                    "${CMAKE_PREFIX_PATH};${LOCAL_DIR}"
                PARENT_SCOPE
                )

            if(DEFINED CMAKE_FIND_ROOT_PATH)
                set (
                    CMAKE_FIND_ROOT_PATH
                        "${CMAKE_FIND_ROOT_PATH};${LOCAL_DIR}"
                    PARENT_SCOPE
                    )
            endif()
        endif()

        set ( ${DEP_ALIAS}_DIR "${LOCAL_DIR}" )

        set ( FILE_URL "${BASE_URL}/${NAME}_${GIT_DEP_BUILDVARIANT}.${DEP_EXTENSION}" )
        set ( LOCAL_FILE "${DEP_CACHE_LOCATION}/${NAME}.${DEP_EXTENSION}" )
        set ( VERSION_FILE "${LOCAL_FILE}.version" )
        set ( TARGET_LOCATION "${DEP_LIB_LOCATION}" )

        if( EXISTS "${VERSION_FILE}" )
            file ( STRINGS "${VERSION_FILE}" LOCAL_VERSION )
        endif()

        #
        # Check for pre-existing download version
        #
        if ( EXISTS "${LOCAL_FILE}" AND "${LOCAL_VERSION}" STREQUAL "${DEP_TAG}" )
            message ( STATUS "${NAME} already up to date (${LOCAL_VERSION})" )
            continue()
        endif()

        message ( STATUS "Downloading ${NAME} (${DEP_TAG})" )

        file ( DOWNLOAD
            "${FILE_URL}"
            "${LOCAL_FILE}"
            STATUS DL_STAT
            )

        list ( GET DL_STAT 0 DL_ERROR )

        if(NOT "${DL_ERROR}" STREQUAL "0" AND ${DEP_REQUIRED})
            message ( STATUS "Error on URL: ${FILE_URL}" )
            message ( FATAL_ERROR "Failed to download ${NAME} (${DEP_TAG}): ${DL_STAT}" )
        endif()

        #
        # Write downloaded version to file
        #
        file ( WRITE "${VERSION_FILE}" "${DEP_TAG}" )

        #
        # Install the library bundle
        #
        execute_process (
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${LOCAL_DIR}"
            )

        execute_process (
            COMMAND ${CMAKE_COMMAND} -E tar xf "${LOCAL_FILE}"
            WORKING_DIRECTORY "${LOCAL_DIR}"
            )
    endforeach()

endfunction()
