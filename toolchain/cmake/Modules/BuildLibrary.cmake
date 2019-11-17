# Contains some code for packaging frameworks
include ( MacAppBuild )
include ( InstallConvenience )

include ( CMakePackageConfigHelpers )

set_property (GLOBAL PROPERTY CF_LIBRARY_DEFINITIONS "" )
set_property (GLOBAL PROPERTY CF_INCLUDE_DIRS "" )

macro ( EXTRACT_HEADER_DIR INPUT_DIR OUTPUT_VAR )
    string ( REGEX REPLACE
        "^.*(INSTALL|BUILD)_INTERFACE:(.*)\>+$"
        "\\2"
        ${OUTPUT_VAR}
        "${INPUT_DIR}"
        )
endmacro()

macro( HEADER_INSTALL LIBNAME INC_DIR BASE_PREFIX )
    if(NOT "${INC_DIR}" STREQUAL "")
        foreach ( INC ${INC_DIR} )
            if("${INC}" MATCHES ".*BUILD_INTERFACE:.*" )
                extract_header_dir ( "${INC}" INC )
                set ( PREFIX include )
                if("${INC}" MATCHES ".*include$")
                    set ( PREFIX )
                endif()
                install (
                    DIRECTORY "${INC}"
                    DESTINATION targets/${LIBNAME}/${PREFIX}/${BASE_PREFIX}
                    )
                string ( REGEX REPLACE ".*\/([^\/]*include)$" "\\1"
                    BASE_SUFFIX "${INC}"
                    )
                if(IS_ABSOLUTE "${BASE_SUFFIX}")
                    set ( BASE_SUFFIX include )
                endif()
                target_include_directories ( ${LIBNAME} INTERFACE
                    $<INSTALL_INTERFACE:targets/${LIBNAME}/${BASE_SUFFIX}>
                    )
            endif()
        endforeach()

    endif()
endmacro()


macro( GENERATE_FINDSCRIPT )
    write_basic_package_version_file (
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        COMPATIBILITY ExactVersion
        )

    install (
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
        )

    install (
        EXPORT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
        )
endmacro()

function( ADD_EXPORT LIB_TARGET )
    if(ANDROID)
        install(
            TARGETS
            ${LIB_TARGET}

            EXPORT ${PROJECT_NAME}

            ARCHIVE DESTINATION "lib/${ANDROID_ABI}"
            LIBRARY DESTINATION "lib/${ANDROID_ABI}"
            RUNTIME DESTINATION "lib/${ANDROID_ABI}"
            PUBLIC_HEADER DESTINATION include/${LIB_TARGET}
            )
    else()
        install(
            TARGETS ${LIB_TARGET}

            EXPORT ${PROJECT_NAME}

            FRAMEWORK DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            ARCHIVE DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            LIBRARY DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            RUNTIME DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            PUBLIC_HEADER DESTINATION include/${LIB_TARGET}

            COMPONENT bin
            )
    endif()
endfunction()

macro(COFFEE_LIBRARY)
    cmake_parse_arguments(
        LIB
        "LINKABLE;NO_EXPORT"
        "TARGET;LINKAGE;VERSION_CODE;COPYRIGHT;COMPANY"
        "SOURCES;LIBRARIES;HEADER_DIRS;HEADER_BASE;RESOURCES;BUNDLE_LIBRARIES;BUNDLE_HEADERS"
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
        header_install ( ${LIB_TARGET} "${LIB_HEADER_DIRS}" "${LIB_HEADER_BASE}" )
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
