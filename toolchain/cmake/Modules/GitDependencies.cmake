set ( GIT_DEP_DIR "${CMAKE_BINARY_DIR}/dependencies" CACHE PATH "" )
set ( GIT_DEP_BUILDVARIANT "" CACHE STRING "" )

option ( GIT_DEP_VERBOSE OFF "Verbose actions for GitDependencies" )

function ( DEPENDENCY_GET )

    cmake_parse_arguments ( DEP
        "REQUIRED"
        "EXTENSION;SOURCE;TAG;CACHE_LOCATION;LIB_LOCATION;SIDELOAD"
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

        set ( "GIT_DEPENDENCIES_${NAME_COMBO}_VERSION" ${DEP_TAG} PARENT_SCOPE )
        set ( "GIT_DEPENDENCIES_${NAME_COMBO}_SOURCE" ${DEP_SOURCE} PARENT_SCOPE )
        set ( "GIT_DEPENDENCIES_${NAME_COMBO}_EXTENSION" ${DEP_EXTENSION} PARENT_SCOPE )

        set ( LIB_HASH "${NAME}:${DEP_TAG}:${DEP_SOURCE}" )

        string ( SHA1 LIB_HASH "${LIB_HASH}" )

        set ( LOCAL_DIR "${DEP_LIB_LOCATION}/${LIB_HASH}" )

        if(NOT "${DEP_SIDELOAD}" STREQUAL "")
            set ( LOCAL_DIR "${DEP_SIDELOAD}" )
        endif()

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

        set ( ${DEP_ALIAS}_DIR "${LOCAL_DIR}" PARENT_SCOPE )

        if(NOT "${DEP_SIDELOAD}" STREQUAL "")
            message ( STATUS "Sideloading ${NAME} from ${DEP_SIDELOAD}" )
            continue()
        endif()

        set ( FILE_URL "${BASE_URL}/${NAME}_${GIT_DEP_BUILDVARIANT}.${DEP_EXTENSION}" )
        set ( LOCAL_FILE "${DEP_CACHE_LOCATION}/${LIB_HASH}.${DEP_EXTENSION}" )
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

function( DEPENDENCY_LINK )

    cmake_parse_arguments ( LINK
        ""
        "TARGET"
        "DEPENDENCIES"

        ${ARGN}
        )

    foreach ( DEP ${LINK_DEPENDENCIES} )
        get_property ( PROPERTY_EXISTS TARGET "${LINK_TARGET}" PROPERTY git_deps SET )

        if(PROPERTY_EXISTS)
            get_property ( GIT_DEPS TARGET "${LINK_TARGET}" PROPERTY git_deps )
            set_property ( TARGET "${LINK_TARGET}" PROPERTY
                git_deps
                ${GIT_DEPS}
                "${DEP},${GIT_DEPENDENCIES_${DEP}_VERSION},${GIT_DEPENDENCIES_${DEP}_SOURCE},${GIT_DEPENDENCIES_${DEP}_EXTENSION}"
                )
        else()
            set_property ( TARGET "${LINK_TARGET}" PROPERTY
                git_deps
                "${DEP},${GIT_DEPENDENCIES_${DEP}_VERSION},${GIT_DEPENDENCIES_${DEP}_SOURCE},${GIT_DEPENDENCIES_${DEP}_EXTENSION}"
                )
        endif()
    endforeach()

    set_property ( TARGET "${LINK_TARGET}" PROPERTY
        EXPORT_PROPERTIES
            git_deps
        )

    get_property ( EXPORT_PROPS TARGET "${LINK_TARGET}" PROPERTY EXPORT_PROPERTIES )

    # message ( "Export props: ${EXPORT_PROPS}" )

endfunction()

set ( GIT_DEPS_RESOLVED ";" )

macro( DEPENDENCY_RESOLVE_INTERNAL )

    cmake_parse_arguments ( RES
        ""
        "TARGET"
        ""

        ${ARGN}
        )


    if(NOT "${GIT_DEPS_RESOLVED}" MATCHES ";${RES_TARGET};"
            AND NOT "${RES_TARGET}" STREQUAL "")

        if(GIT_DEP_VERBOSE)
            message ( STATUS "Resolving dependency ${RES_TARGET}" )
        endif()

        set ( GIT_DEPS_RESOLVED "${GIT_DEPS_RESOLVED}${RES_TARGET};" )

        get_target_property( "LIB_TYPE" "${RES_TARGET}" TYPE )

        if(NOT "${LIB_TYPE}" STREQUAL "INTERFACE_LIBRARY")
            get_property ( GIT_DEPS_DEFINED TARGET "${RES_TARGET}" PROPERTY git_deps SET )

            if(GIT_DEPS_DEFINED)
                get_property ( GIT_DEPS TARGET "${RES_TARGET}" PROPERTY git_deps )

                if(GIT_DEP_VERBOSE)
                    message ( STATUS "${RES_TARGET} depends on: ${GIT_DEPS}" )
                endif()

                foreach( DEP ${GIT_DEPS} )
                    if("${DEP}" STREQUAL "")
                        continue()
                    endif()

                    string ( REPLACE "," ";" DEP_DATA "${DEP}" )
                    list ( GET DEP_DATA 0 NAME )
                    list ( GET DEP_DATA 1 VERSION )
                    list ( GET DEP_DATA 2 SOURCE )
                    list ( GET DEP_DATA 3 EXTENSION )

                    string ( REPLACE "=" ";" NAME_LIST "${NAME}" )
                    list ( LENGTH NAME_LIST NAME_LEN )
                    if(${NAME_LEN} GREATER 1)
                        list ( GET NAME_LIST 1 DEP_NAME )
                    else()
                        set ( DEP_NAME "${NAME}" )
                    endif()

                    dependency_get (
                        REQUIRED
                        SOURCE ${SOURCE}
                        TAG ${VERSION}
                        NAMES ${NAME}
                        EXTENSION ${EXTENSION}
                        SIDELOAD "${${DEP_NAME}_LOCAL_BUILD}"
                        )
                endforeach()
            endif()
        endif()

        get_property ( LINK_LIBS_DEFINED TARGET "${RES_TARGET}" PROPERTY INTERFACE_LINK_LIBRARIES SET )

        if(LINK_LIBS_DEFINED)
            get_property ( LINK_LIBS TARGET "${RES_TARGET}" PROPERTY INTERFACE_LINK_LIBRARIES )

            foreach( LIB ${LINK_LIBS} )
                if(NOT TARGET "${LIB}" AND NOT "${LIB}" MATCHES "-framework")
                    string ( REPLACE "::" ";" LIB_LIST "${LIB}" )
                    list ( LENGTH LIB_LIST LIB_LEN )
                    if(${LIB_LEN} LESS 2)
                        continue()
                    endif()

                    if(GIT_DEP_VERBOSE)
                        message ( STATUS "Finding package ${LIB_PACKAGE}" )
                    endif()

                    list ( GET LIB_LIST 0 LIB_PACKAGE )
                    find_package( ${LIB_PACKAGE} REQUIRED )
                endif()

                if(TARGET "${LIB}")
                    dependency_resolve_internal ( TARGET ${LIB} )
                endif()
            endforeach()
        endif()
    endif()
endmacro()

macro( DEPENDENCY_RESOLVE )
    foreach ( DEP ${ARGN} )
        dependency_resolve_internal ( TARGET ${DEP} )
    endforeach()
endmacro()
