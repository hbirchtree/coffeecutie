################################################################################
# For embedding license info and application info
################################################################################

macro(COFFEE_GEN_LICENSEINFO TARGET LICENSES)
    set ( LICENSE_FILE "${CMAKE_CURRENT_BINARY_DIR}/LicenseInfo_${TARGET}.c" )

    set ( LICENSE_BUNDLE "${CMAKE_SOURCE_DIR}/LICENSE;${LICENSES}" )

    set ( LICENSE_COUNT 0 )

    foreach(licsn ${LICENSE_BUNDLE})
        file ( READ "${licsn}" THIS_LICENSE )
        string ( REPLACE "\"" "\\\"" THIS_LICENSE "${THIS_LICENSE}" )
        string ( REPLACE "\n" "\\n\"\n\"" THIS_LICENSE "${THIS_LICENSE}" )
        if(NOT LICENSE_DATA)
            set ( LICENSE_DATA "\"${THIS_LICENSE}\"" )
        else()
            set ( LICENSE_DATA "${LICENSE_DATA},\"${THIS_LICENSE}\"" )
        endif()
        math( EXPR LICENSE_COUNT "${LICENSE_COUNT}+1" )
    endforeach()

    configure_file (
        "${COFFEE_SOURCE_TEMPLATE_DIRECTORY}/LicenseInfo.c.in"
        "${LICENSE_FILE}"
        )
endmacro()

macro(COFFEE_GEN_APPLICATIONINFO TARGET TITLE COMPANY VERSION)
    set ( APP_TITLE "${TITLE}" )
    set ( APP_COMPANY "${COMPANY}" )
    set ( APP_VERSION "${VERSION}" )

    set ( APPLICATION_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/AppInfo_${TARGET}.cpp" )

    configure_file (
        "${COFFEE_SOURCE_TEMPLATE_DIRECTORY}/AppInfo.cpp.in"
        "${APPLICATION_INFO_FILE}"
        )
endmacro()

macro(GEN_VERSION_HEADER TARGET)
    set ( VERSION_FILE "module_version.h" )
    configure_file (
        "${COFFEE_SOURCE_TEMPLATE_DIRECTORY}/VersionWriter.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_Generate.cmake"
        @ONLY
        )

    set ( VERSION_FILE_FULL "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE}" )
    add_custom_command (
        OUTPUT "${VERSION_FILE_FULL}"

        COMMAND
            "${CMAKE_COMMAND}"
                -P "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_Generate.cmake"
        DEPENDS
            ${CMAKE_SOURCE_DIR}/.build.yml
            ${CMAKE_SOURCE_DIR}/.git
        )
    add_custom_target ( ${TARGET}_module_version
        DEPENDS "${VERSION_FILE_FULL}"
        )
    add_dependencies ( ${TARGET} "${TARGET}_module_version" )
    target_include_directories ( ${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
endmacro()
