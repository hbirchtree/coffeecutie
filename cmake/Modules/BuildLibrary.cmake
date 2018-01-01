# Contains some code for packaging frameworks
include ( MacAppBuild )

set_property (GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS "" )
set_property (GLOBAL PROPERTY CF_INCLUDE_DIRS "" )

macro( REGISTER_LIBRARY LIBNAME INC_DIR )
    get_property( LIBRARY_DEFINITIONS GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS )
    set_property(GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS "${LIBNAME};${LIBRARY_DEFINITIONS}" )

    get_property( INC_DIRS GLOBAL PROPERTY CF_INCLUDE_DIRS )
    set_property(GLOBAL PROPERTY CF_INCLUDE_DIRS "${INC_DIR};${INC_DIRS}" )
endmacro()

macro( GENERATE_FINDSCRIPT )
    get_property( LIBRARY_DEFINITIONS GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS )
    get_property( INC_DIRS_ GLOBAL PROPERTY CF_INCLUDE_DIRS )

    get_property( COMP_DEFS_RAW DIRECTORY . PROPERTY COMPILE_DEFINITIONS )

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
        get_property ( LIB_INCLUDES TARGET ${LIBRARY} PROPERTY INCLUDE_DIRECTORIES )
        foreach(INC_PATH ${LIB_INCLUDES})
            set ( INC_DIRS "${INC_DIRS};${INC_PATH}" )
        endforeach()
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

    export ( PACKAGE ${PROJECT_NAME} )

    export (
        TARGETS ${LIBRARY_DEFINITIONS}
#        NAMESPACE ${PROJECT_NAME}::
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
        )

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/Templates/Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY
        )
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/Templates/ConfigVersion.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        @ONLY
        )

    install (
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION share
        )

    install (
        EXPORT ${PROJECT_NAME}
#        NAMESPACE ${PROJECT_NAME}::
        DESTINATION share
        )
endmacro()

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES LIBRARIES HEADER_DIR)
    if(IS_DIRECTORY "${HEADER_DIR}")
        file ( GLOB_RECURSE ALL_HEADERS
            #        ${HEADER_DIR}/*.h
            #        ${HEADER_DIR}/*.hpp
            ${HEADER_DIR}/*
            )
    endif()
    source_group ( "${TARGET}_headers" FILES ${ALL_HEADERS} )
    
    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}" "${ALL_HEADERS}")

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    if(ALL_HEADERS)
        set_property(TARGET ${TARGET} PROPERTY PUBLIC_HEADER ${ALL_HEADERS} )
    endif()

    if(APPLE)
        set_target_properties( ${TARGET} PROPERTIES MACOSX_RPATH "." )
    elseif(WIN32)
        set_target_properties ( ${TARGET}
            PROPERTIES
            VERSION ${COFFEE_BUILD_STRING}
            SOVERSION 1
            )
    endif()

    target_enable_cxx11(${TARGET})

    target_link_libraries(${TARGET}
        PUBLIC
        ${LIBRARIES}
        )
    target_compile_definitions ( ${TARGET}
        PRIVATE
        -DCOFFEE_APPLICATION_LIBRARY
        )

    file ( WRITE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET}.link" "${LIBRARIES}" )

    if(ANDROID)
        install(
            TARGETS
            ${TARGET}

            EXPORT ${PROJECT_NAME}

            ARCHIVE DESTINATION "lib/${ANDROID_ABI}"
            LIBRARY DESTINATION "lib/${ANDROID_ABI}"
            RUNTIME DESTINATION "lib/${ANDROID_ABI}"
            PUBLIC_HEADER DESTINATION include
            )
    else()
        install(
            TARGETS ${TARGET}

            EXPORT ${PROJECT_NAME}

            ARCHIVE DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            LIBRARY DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            RUNTIME DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            PUBLIC_HEADER DESTINATION include
            )
    endif()

    register_library( ${TARGET} "${HEADER_DIR}" )
endmacro()

macro(COFFEE_ADD_FRAMEWORK
        TARGET LINKOPT
        VERSION_CODE COPYRIGHT COMPANY
        SOURCES HEADER_DIR
        BUNDLE_RSRCS BUNDLE_HDRS
        LIBRARIES BUNDLE_LIBRARIES)
    if(APPLE AND NOT IOS)
        if(IS_DIRECTORY "${HEADER_DIR}")
            file ( GLOB_RECURSE ${TARGET}_HEADERS
                ${HEADER_DIR}/*
                )
        endif()
        source_group ( "${TARGET}_headers" FILES ${ALL_HEADERS} )

        MACFRAMEWORK_PACKAGE(
            "${TARGET}" "${LINKOPT}"
            "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${SOURCES};${${TARGET}_HEADERS}" "${BUNDLE_RSRCS}" "${BUNDLE_HDRS}")

        target_link_libraries(${TARGET}
            PUBLIC
            ${LIBRARIES}
            )
        target_compile_definitions ( ${TARGET}
            PRIVATE
            -DCOFFEE_APPLICATION_LIBRARY
            )

        if(ALL_HEADERS)
            set_property(TARGET ${TARGET} PROPERTY PUBLIC_HEADER ${ALL_HEADERS} )
        endif()

        register_library( ${TARGET} "${HEADER_DIR}" )
    else()
        coffee_add_elibrary(
            "${TARGET}" "${LINKOPT}"
            "${SOURCES}" "${LIBRARIES}" "${HEADER_DIR}")
    endif()

    target_compile_definitions ( ${TARGET}
        PRIVATE
        -DCOFFEE_COMPONENT_NAME="${TARGET}"
        )

    target_enable_cxx11(${TARGET})
endmacro()

macro(COFFEE_ADD_LIBRARY TARGET SOURCES LIBRARIES HEADER_DIR)
    # Just a little simplification
    coffee_add_framework(
        ${TARGET} ${COFFEE_LINK_OPT}
        "2.0" "hbirchtree" "hbirchtree"
        "${SOURCES}" "${HEADER_DIR}" "" ""
        "${LIBRARIES}" "")
endmacro()
