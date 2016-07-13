if(ANDROID)

endif()

include ( LinuxAppImageBuild )
include ( LinuxFlatpakBuild )
include ( MacAppBuild )
include ( WindowsImagePacker )

# Wrappers to get rid of boilerplate and cross-platform-ness (ahem, Android)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID)
        # Android's compiler doesn't support target_compile_features :(
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    elseif(APPLE)
        # To get around old Clang versions
        set(CMAKE_CXX_FLAGS "-stdlib=libc++ -std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()
endmacro()

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES)
    add_definitions( -DCOFFEE_APPLICATION_LIBRARY )

    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}")

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    target_enable_cxx11(${TARGET})

    if(APPLE)
        set_target_properties( ${TARGET} PROPERTIES MACOSX_RPATH "." )
    elseif(WIN32)
        set_target_properties ( ${TARGET}
            PROPERTIES
            VERSION ${COFFEE_BUILD_STRING}
            SOVERSION 1
            )
    endif()

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        lib
        )

endmacro()

macro(COFFEE_ADD_LIBRARY TARGET SOURCES)
    # Just a little simplification
    coffee_add_elibrary(${TARGET} ${COFFEE_LINK_OPT} "${SOURCES}")
endmacro()

# TODO: Make package name more configurable
# Android only uses shared libraries which are loaded, all else uses typical executables
# We do a test to check if a library is a shared library for Android
# For iOS, everything will be statically linked, which might be used for Android as well.
# For now, we leave the linking options here for desktop platforms
macro(COFFEE_ADD_EXAMPLE_LONGER
        TARGET
        TITLE APP_COMPANY_NAME APP_VERSION_CODE
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES)

    add_definitions( -DCOFFEE_APPLICATION_NAME="${TITLE}" )
    add_definitions( -DCOFFEE_ORGANIZATION_NAME="${APP_COMPANY_NAME}" )
    add_definitions( -DCOFFEE_VERSION_CODE=${APP_VERSION_CODE} )

    set ( LICENSE_FILE "${CMAKE_CURRENT_BINARY_DIR}/LicenseInfo_${TARGET}.cpp" )
    # TODO: Load license data from files, references stored in BUNDLE_LICENSES
    set ( LICENSE_DATA
        "Hello!"
        "Jello!"
        "What?"
        )

    file ( WRITE "${LICENSE_FILE}"
        "namespace Coffee{\n"
        "  const char* CoffeeLicenseString = \"${LICENSE_DATA}\";\n"
        "}" )

    set ( SOURCES_MOD "${LICENSE_FILE};${SOURCES}" )

    set ( COMPANY "${APP_COMPANY_NAME}" )
    set ( INFO_STRING "It's a Coffee application" )
    set ( COPYRIGHT "Coffeecutie, MIT license" )
    set ( VERSION_CODE "${APP_VERSION_CODE}" )

    set ( PACKAGE_PREFIX "org.coffee" )

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
            "${BUNDLE_RSRCS}" )
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

        if(COFFEE_GENERATE_APPIMAGE)
            APPIMAGE_PACKAGE(
                ${TARGET}
                "${TITLE}"
                "${BUNDLE_RSRCS}" "" "${BUNDLE_LIBS}")
        endif()
        if(COFFEE_GENERATE_FLATPAK)
            FLATPAK_PACKAGE(
                ${TARGET}
                "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
                "${BUNDLE_RSRCS}"
                "" "${BUNDLE_LIBS}" )
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
endmacro()

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

macro(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    add_definitions( -DCOFFEE_APPLICATION_NAME="${TITLE}" )
    add_definitions( -DCOFFEE_ORGANIZATION_NAME="Coffecutie" )
    add_definitions( -DCOFFEE_VERSION_CODE=1 )

    set ( LICENSE_FILE "${CMAKE_CURRENT_BINARY_DIR}/LicenseInfo_${TARGET}.cpp" )
    set ( LICENSE_DATA
        "Hello!"
        "Jello!"
        "What?"
        )

    file ( WRITE "${LICENSE_FILE}"
        "namespace Coffee{\n"
        "  const char* CoffeeLicenseString = \"${LICENSE_DATA}\";\n"
        "}" )

    set ( SOURCES_MOD "${LICENSE_FILE};${SOURCES}" )

    add_executable ( ${TARGET} ${SOURCES_MOD} )

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        tests
        )

    target_link_libraries ( ${TARGET}
	${LIBRARIES}
	)

    target_enable_cxx11(${TARGET})

    add_test (
	NAME ${TITLE}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND $<TARGET_FILE:${TARGET}>
        )
endmacro()
