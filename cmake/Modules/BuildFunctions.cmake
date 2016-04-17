if(ANDROID)
    include ( AndroidToolkit )
    include ( AndroidApkBuild )
    find_package(SDL2main REQUIRED)
endif()

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
    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}")
    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_enable_cxx11(${TARGET})

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

include ( GetPrerequisites )

# TODO: Make package name more configurable
# Android only uses shared libraries which are loaded, all else uses typical executables
# We do a test to check if a library is a shared library for Android
# For iOS, everything will be statically linked, which might be used for Android as well.
# For now, we leave the linking options here for desktop platforms
macro(COFFEE_ADD_EXAMPLE TARGET TITLE SOURCES LIBRARIES)
    if(ANDROID)
        add_library(${TARGET} SHARED ${SOURCES} "${SDL2_ANDROID_MAIN_FILE}" )
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    elseif(WIN32)
        add_executable(${TARGET} ${SOURCES} ${CMAKE_SOURCE_DIR}/desktop/windows/winresources.rc )
        set_target_properties ( ${TARGET}
            PROPERTIES
            VERSION ${COFFEE_BUILD_STRING}
            SOVERSION 1
            )
    elseif(APPLE)
        set ( OSX_ICON "${CMAKE_SOURCE_DIR}/desktop/osx/Coffee.icns" )

        set_source_files_properties ( "${OSX_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources" )

        set ( MACOSX_BUNDLE_LONG_VERSION_STRING ${COFFEE_BUILD_STRING} )
        set ( MACOSX_BUNDLE_BUNDLE_VERSION "${COFFEE_BUILD_STRING}" )
        set ( MACOSX_BUNDLE_SHORT_VERSION_STRING ${COFFEE_VERSION_CODE} )
        set ( MACOSX_BUNDLE_ICON_FILE "Coffee.icns" )

        set ( MACOSX_BUNDLE_COPYRIGHT "Some of this code is under the MIT license" )
        set ( MACOSX_BUNDLE_BUNDLE_NAME "${TITLE}" )
        set ( MACOSX_BUNDLE_INFO_STRING "Coffeecutie Game" )
        set ( MACOSX_BUNDLE_GUI_IDENTIFIER "${TITLE}" )

        add_executable(${TARGET} MACOSX_BUNDLE ${SOURCES})
    else()
        add_executable(${TARGET} ${SOURCES})
    endif()
    target_enable_cxx11(${TARGET})

    target_link_libraries ( ${TARGET}
        ${LIBRARIES}
        )

    if(APPLE)
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            app
            )
    else()
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            bin
            )
    endif()

    if(ANDROID)

        # Lowercase the target name ofr package name
        string ( TOLOWER "${TARGET}" PACKAGE_SUFFIX )

        set ( DEPENDENCIES )

        list ( APPEND DEPENDENCIES $<TARGET_FILE:${TARGET}> )

        list ( APPEND DEPENDENCIES "${ARGN}" )

        package_apk(
            "${TARGET}"
            "${TITLE}"
            "org.coffee.${PACKAGE_SUFFIX}"
            "${COFFEE_VERSION_CODE}" "${COFFEE_BUILD_STRING}"
            "${ANDROID_NATIVE_API_LEVEL}" "${ANDROID_ABI}"
            "${DEPENDENCIES}" )

    endif()
endmacro()
