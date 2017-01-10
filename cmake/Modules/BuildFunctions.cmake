include ( AndroidApkBuild )
include ( LinuxAppImageBuild )
include ( LinuxFlatpakBuild )
include ( LinuxSnappyBuild )
include ( MacAppBuild )
include ( WindowsImageBuild )

include ( ValgrindTest )

# Wrappers to get rid of boilerplate and cross-platform-ness (ahem, Android)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(NOT ANDROID AND NOT APPLE)
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()
endmacro()

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

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES LIBRARIES HEADER_DIR)
    add_definitions( -DCOFFEE_APPLICATION_LIBRARY )

    file ( GLOB_RECURSE ${TARGET}_HEADERS
        ${HEADER_DIR}/*.h
        ${HEADER_DIR}/*.hpp
        )
    source_group ( "${TARGET}_headers" FILES ${ALL_HEADERS} )

    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}" "${${TARGET}_HEADERS}")

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

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

    target_link_libraries(${TARGET} ${LIBRARIES})

    file ( WRITE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET}.link" "${LIBRARIES}" )

    if(ANDROID)
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            lib/${ANDROID_ABI}
            )
    elseif(NOT WIN32)
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            lib
            )
    else()
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            bin
            )
    endif()

#    install (
#        FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET}.libraries"
#        DESTINATION deps)
endmacro()

macro(COFFEE_ADD_FRAMEWORK
        TARGET LINKOPT
        VERSION_CODE COPYRIGHT COMPANY
        SOURCES HEADER_DIR
        BUNDLE_RSRCS BUNDLE_HDRS
        LIBRARIES BUNDLE_LIBRARIES)
    if(APPLE)
        MACFRAMEWORK_PACKAGE(
            "${TARGET}" "${LINKOPT}"
            "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${SOURCES}" "${BUNDLE_RSRCS}" "${BUNDLE_HDRS}")
        target_link_libraries(${TARGET} ${LIBRARIES})
    else()
        coffee_add_elibrary(
            "${TARGET}" "${LINKOPT}"
            "${SOURCES}" "${LIBRARIES}" "${HEADER_DIR}")
    endif()

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


# TODO: Make package name more configurable
# Android only uses shared libraries which are loaded, all else uses typical executables
# We do a test to check if a library is a shared library for Android
# For iOS, everything will be statically linked, which might be used for Android as well.
# For now, we leave the linking options here for desktop platforms
function(COFFEE_ADD_APPLICATION_LONGERER
        TARGET
        TITLE APP_COMPANY_NAME APP_VERSION_CODE
        INFO_STRING COPYRIGHT
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES
        PERMISSIONS
        BUNDLE_BINARIES)

    coffee_gen_licenseinfo("${TARGET}" "${BUNDLE_LICENSES}")
    coffee_gen_applicationinfo("${TARGET}"
        "${TITLE}"
        "${APP_COMPANY_NAME}"
        "${APP_VERSION_CODE}"
        )

    set ( SOURCES_MOD "${APPLICATION_INFO_FILE};${LICENSE_FILE};${SOURCES}" )

    set ( COMPANY "${APP_COMPANY_NAME}" )
    set ( VERSION_CODE "${APP_VERSION_CODE}" )

    set ( PACKAGE_PREFIX "org.coffee" )

    set ( ICON_ASSET "${COFFEE_DESKTOP_DIRECTORY}/icon.svg" )

    if(ANDROID)
        ANDROIDAPK_PACKAGE(
            "${TARGET}"
            "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${BUNDLE_LIBS}"
            "${ICON_ASSET}" )
    elseif(WIN32)
        WINPE_PACKAGE(
            ${TARGET}
            "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${INFO_STRING}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${ICON_ASSET}")
    elseif(APPLE)
        MACAPP_PACKAGE(
            "${TARGET}"
            "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${INFO_STRING}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${BUNDLE_LIBS}"
            "${ICON_ASSET}" )
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_executable( ${TARGET} ${SOURCES_MOD} )

        # We still install a basic binary executable
        install(
            FILES
            "$<TARGET_FILE:${TARGET}>"

            DESTINATION
            bin
            )

        if(COFFEE_GENERATE_APPIMAGE)
            APPIMAGE_PACKAGE(
                ${TARGET}
                "${TITLE}"
                "${BUNDLE_RSRCS}" "" "${BUNDLE_LIBS}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_FLATPAK)
            FLATPAK_PACKAGE(
                ${TARGET}
                "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
                "${BUNDLE_RSRCS}"
                "" "${BUNDLE_LIBS}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_SNAPPY)
            SNAPPY_PACKAGE(
                ${TARGET}
                "${TITLE}" "${INFO_STRING}"
                "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
                "${BUNDLE_RSRCS}"
                "" "${BUNDLE_LIBS}"
                "${BUNDLE_BINARIES}"
                "${ICON_ASSET}"
                "${PERMISSIONS}"
                )
        endif()

    else()
        add_executable(${TARGET} ${SOURCES_MOD})

        install(
            TARGETS
            ${TARGET}

            DESTINATION
            bin
            )
    endif()

    target_enable_cxx11(${TARGET})

    target_link_libraries ( ${TARGET}
        ${LIBRARIES}
        )

    remove_definitions(
        -DCOFFEE_APPLICATION_NAME -DCOFFEE_ORGANIZATION_NAME
        -DCOFFEE_VERSION_CODE )

endfunction()

# Wrapper functions

macro(COFFEE_ADD_APPLICATION_LONGER
        TARGET
        TITLE COMPANY VERSION
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS)

    set ( PERMISSIONS "OPENGL;AUDIO;ENVIRONMENT_SENSORS;NETWORK_CONNECT;JOYSTICK" )
    set ( INFO_STRING "It's a Coffee application" )
    set ( COPYRIGHT "Coffeecutie, MIT license" )

    COFFEE_ADD_APPLICATION_LONGERER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${INFO_STRING}" "${COPYRIGHT}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}"
        "${PERMISSIONS}"
        "")
endmacro()

function(COFFEE_ADD_EXAMPLE_LONGER
        TARGET
        TITLE APP_COMPANY_NAME APP_VERSION_CODE
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES)
    COFFEE_ADD_APPLICATION_LONGER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}")
endfunction()

macro(COFFEE_ADD_EXAMPLE_LONG
        TARGET
        TITLE
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS)

    set ( VERSION 5 )
    set ( COMPANY "Coffee Guy" )

    COFFEE_ADD_EXAMPLE_LONGER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}")
endmacro()

macro(COFFEE_ADD_APPLICATION_LONG TARGET TITLE SOURCES LIBRARIES BUNDLE_LIBS BUNDLE_RSRCS)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}")
endmacro()

macro(COFFEE_ADD_EXAMPLE TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()

macro(COFFEE_ADD_APPLICATION TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()

# CTest functions

function(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    # Bleh, Android kind of sucks for this.
    if(ANDROID)
        return()
    endif()

    coffee_gen_licenseinfo("${TARGET}" "")
    coffee_gen_applicationinfo("${TARGET}"
        "${TITLE}"
        "Coffee"
        "5"
        )

    if(ANDROID) # Android platform layer
        set ( SOURCES_MOD "${SOURCES};${APPLICATION_INFO_FILE};${LICENSE_FILE};${SDL2_ANDROID_MAIN_FILE}" )
    else()
        set ( SOURCES_MOD "${SOURCES};${APPLICATION_INFO_FILE};${LICENSE_FILE}" )
    endif()

    add_executable ( ${TARGET} ${SOURCES_MOD} )

    install(
        FILES
        "$<TARGET_FILE:${TARGET}>"

        DESTINATION
        bin/tests
        )

    target_link_libraries ( ${TARGET}
        ${LIBRARIES}
        )

    target_enable_cxx11(${TARGET})

    if(ANDROID) # Crosscompiling setup, until we find an elegant solution
        message ( "Skipping unit test: ${TITLE}" )
        message ( "Please run the tests somehow!" )
    else()
        add_test (
            NAME ${TITLE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND $<TARGET_FILE:${TARGET}>
            )
    endif()

    if(COFFEE_VALGRIND_MASSIF)
        VALGRIND_TEST("massif" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_MEMCHECK)
        VALGRIND_TEST("memcheck" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_CALLGRIND)
        VALGRIND_TEST("callgrind" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_CACHEGRIND)
        VALGRIND_TEST("cachegrind" "--stacks=yes" "${TARGET}")
    endif()

endfunction()
