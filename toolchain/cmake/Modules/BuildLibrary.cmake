# Contains some code for packaging frameworks
include ( MacAppBuild )
include ( InstallConvenience )

set_property (GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS "" )
set_property (GLOBAL PROPERTY CF_INCLUDE_DIRS "" )

macro( REGISTER_LIBRARY LIBNAME INC_DIR )
    get_property( LIBRARY_DEFINITIONS GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS )
    set_property(GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS "${LIBNAME};${LIBRARY_DEFINITIONS}" )

#    get_property( INC_DIRS GLOBAL PROPERTY CF_INCLUDE_DIRS )
#    set_property(GLOBAL PROPERTY CF_INCLUDE_DIRS "${INC_DIR};${INC_DIRS}" )
    foreach ( INC ${INC_DIR} )
        if("${INC}" MATCHES ".*BUILD_INTERFACE:.*" )
            string ( REGEX REPLACE "^.*BUILD_INTERFACE:(.*)\>+$" "\\1"
                INC_
                "${INC}"
                )
            string ( REPLACE ">" "" INC_ "${INC_}" )
            coffee_bundle (
                HEADER_DIRECTORIES
                ${INC_}
                )
        endif()
    endforeach()
endmacro()

macro ( EXTRACT_HEADER_DIR INPUT_DIR OUTPUT_VAR )
    string ( REGEX REPLACE
        "^.*(INSTALL|BUILD)_INTERFACE:(.*)\>+$"
        "\\2"
        ${OUTPUT_VAR}
        "${INPUT_DIR}"
        )
endmacro()

macro( GENERATE_FINDSCRIPT )
    get_property( LIBRARY_DEFINITIONS GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS )
    get_property( INC_DIRS_ GLOBAL PROPERTY CF_INCLUDE_DIRS )

    get_property( COMP_DEFS_RAW DIRECTORY . PROPERTY COMPILE_DEFINITIONS )

    if ( "${LIBRARY_DEFINITIONS}" STREQUAL "" )
        return()
    endif()

    set ( CONFIG_H "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Def.h" )
    file ( WRITE "${CONFIG_H}" "#pragma once\n\n" )
    foreach ( DEF ${COMP_DEFS_RAW} )
        if( "${DEF}" MATCHES "^.*=.*$" )
            string ( REGEX REPLACE
                "^(.*)=(.*)$" "\\1 \\2"
                DEF "${DEF}"
                )
        endif()
        file ( APPEND "${CONFIG_H}"
            "#define ${DEF}\n\n"
            )
    endforeach()

    install (
        FILES ${CONFIG_H}
        DESTINATION include
        )

    foreach(LIBRARY ${LIBRARY_DEFINITIONS})
        get_property ( LIB_TYPE TARGET ${LIBRARY} PROPERTY
            TYPE )

        if(NOT "${LIB_TYPE}" STREQUAL "INTERFACE_LIBRARY")
            get_property ( LIB_INCLUDES TARGET ${LIBRARY}
                PROPERTY INCLUDE_DIRECTORIES )
            foreach(INC_PATH ${LIB_INCLUDES})
                set ( INC_DIRS "${INC_DIRS};${INC_PATH}" )
            endforeach()
        endif()
    endforeach()

    set ( CONF_INCLUDE_DIRS "" )
    # Create a client-reproducible form of the incude directories for deploy
    foreach ( INC ${INC_DIRS} )
        if(NOT "${INC}" MATCHES ".*BUILD_INTERFACE:.*" )
            string ( REGEX REPLACE "^.*INSTALL_INTERFACE:(.*)\>+$" "\${COFFEE_ROOT_DIR}/\\1"
                INC_
                "${INC}"
                )
            string ( REPLACE ">" "" INC_ "${INC_}" )
            if(NOT IS_ABSOLUTE "${INC_}")
                set ( CONF_INCLUDE_DIRS "${INC_};${CONF_INCLUDE_DIRS}" )
            endif()
        endif()
    endforeach()

    # Dedupe it, because it's a lot
    list( REMOVE_DUPLICATES CONF_INCLUDE_DIRS )

    export (
        TARGETS ${LIBRARY_DEFINITIONS}
        NAMESPACE ${PROJECT_NAME}::
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
        )

    configure_file(
        "${COFFEE_CMAKE_TEMPLATE_DIR}/Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY
        )
    configure_file(
        "${COFFEE_CMAKE_TEMPLATE_DIR}/ConfigVersion.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        @ONLY
        )

    install (
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
        )

    install (
        EXPORT ${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
        )
endmacro()

function( ADD_EXPORT LIB_TARGET LIB_HEADER_DIRS )
    if(ANDROID)
        install(
            TARGETS
            ${LIB_TARGET}

            EXPORT ${PROJECT_NAME}

            ARCHIVE DESTINATION "lib/${ANDROID_ABI}"
            LIBRARY DESTINATION "lib/${ANDROID_ABI}"
            RUNTIME DESTINATION "lib/${ANDROID_ABI}"
            PUBLIC_HEADER DESTINATION include
            )
    else()
        install(
            TARGETS ${LIB_TARGET}

            EXPORT ${PROJECT_NAME}

            FRAMEWORK DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            ARCHIVE DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            LIBRARY DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            RUNTIME DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            PUBLIC_HEADER DESTINATION include

            COMPONENT bin
            )
    endif()

    register_library( ${LIB_TARGET} "${LIB_HEADER_DIRS}" )

endfunction()

macro(COFFEE_LIBRARY)
    cmake_parse_arguments(
        LIB
        "LINKABLE;NO_EXPORT"
        "TARGET;LINKAGE;VERSION_CODE;COPYRIGHT;COMPANY"
        "SOURCES;LIBRARIES;HEADER_DIRS;RESOURCES;BUNDLE_LIBRARIES;BUNDLE_HEADERS"
        ${ARGN}
        )

    if(NOT DEFINED LIB_LINKAGE)
        set ( LIB_LINKAGE ${COFFEE_LINK_OPT} )
    endif()

    if(NOT DEFINED LIB_VERSION_CODE)
        set ( LIB_VERSION_CODE "1" )
    endif()

    if(NOT DEFINED LIB_COPYRIGHT)
        set ( LIB_COPYRIGHT "hbirchtree" )
    endif()

    if(NOT DEFINED LIB_COMPANY)
        set ( LIB_COMPANY "hbirchtree" )
    endif()

    foreach( HEADER_DIR ${LIB_HEADER_DIRS} )
        if("${HEADER_DIR}" MATCHES ".*BUILD_INTERFACE:.*")
            extract_header_dir ( ${HEADER_DIR} HEAD_DIR )

            if(IS_DIRECTORY "${HEAD_DIR}")
                file ( GLOB_RECURSE ALL_HEADERS
                    #        ${HEADER_DIR}/*.h
                    #        ${HEADER_DIR}/*.hpp
                    ${HEADER_DIR}/*
                    )
            else()
                message ( FATAL_ERROR "No header directory: ${HEAD_DIR} (${HEADER_DIR})" )
            endif()
        endif()
    endforeach()

    source_group ( "${LIB_TARGET}_headers" FILES ${ALL_HEADERS} )

    if(APPLE AND NOT IOS AND NOT LIB_LINKABLE)

        MACFRAMEWORK_PACKAGE(
            "${LIB_TARGET}" "${LIB_LINKAGE}"
            "${LIB_VERSION_CODE}" "${LIB_COPYRIGHT}" "${LIB_COMPANY}"
            "${LIB_SOURCES};${ALL_HEADERS}" "${LIB_RESOURCES}"
            "${LIB_BUNDLE_HEADERS}"
            )

    else()

        add_library(${LIB_TARGET} ${LIB_LINKAGE}
            ${LIB_SOURCES}
            ${ALL_HEADERS}
            )

        set_property(TARGET ${LIB_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

        if(APPLE)
            set_target_properties( ${LIB_TARGET} PROPERTIES MACOSX_RPATH "." )
        elseif(WIN32)
            set_target_properties ( ${LIB_TARGET}
                PROPERTIES
                VERSION ${COFFEE_BUILD_STRING}
                SOVERSION 1
                )
        endif()

    endif()

    target_enable_cxx11(${LIB_TARGET})

    target_include_directories ( ${LIB_TARGET}
        PUBLIC
        ${LIB_HEADER_DIRS}
        )

    target_link_libraries( ${LIB_TARGET}
        PUBLIC
        ${LIB_LIBRARIES}
        )

    target_compile_definitions ( ${LIB_TARGET}
        PRIVATE
        -DCOFFEE_APPLICATION_LIBRARY
        -DCOFFEE_COMPONENT_NAME="${LIB_TARGET}"
        )

    target_compile_definitions ( ${LIB_TARGET}
        PUBLIC
        -DFEATURE_ENABLE_${LIB_TARGET}=1
        )

    if(NOT LIB_NO_EXPORT)
        add_export ( ${LIB_TARGET} "${LIB_HEADER_DIRS}" )
    endif()

endmacro()

macro(COFFEE_FRAMEWORK)
    cmake_parse_arguments(
        LIB
        ""
        "TARGET;LINKAGE;VERSION_CODE;COPYRIGHT;COMPANY"
        "SOURCES;LIBRARIES;HEADER_DIRS;RESOURCES;BUNDLE_LIBRARIES;BUNDLE_HEADERS"
        ${ARGN}
        )

    coffee_library(
        TARGET "${LIB_TARGET}"
        LINKAGE "${LIB_LINKAGE}"
        VERSION_CODE "${LIB_VERSION_CODE}"
        COPYRIGHT "${LIB_COPYRIGHT}"
        COMPANY "${LIB_COMPANY}"
        SOURCES ${LIB_SOURCES}
        LIBRARIES ${LIB_LIBRARIES}
        HEADER_DIRS ${LIB_HEADER_DIRS}
        RESOURCES ${LIB_RESOURCES}
        BUNDLE_LIBRARIES ${LIB_BUNDLE_LIBRARIES}
        BUNDLE_HEADERS ${LIB_BUNDLE_HEADERS}
        )

endmacro()
