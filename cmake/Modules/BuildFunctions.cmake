if(ANDROID)
    find_package ( CfAndroidMain )
    include ( AndroidToolkit )
    include ( AndroidApkBuild )

    if(ANDROID_USE_SDL2_LAUNCH)
        find_package(SDL2main REQUIRED)
    endif()
endif()

include ( LinuxAppImageBuild )
include ( WindowsImagePacker )

# Wrappers to get rid of boilerplate and cross-platform-ness (ahem, Android)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID OR "${CMAKE_MAJOR_VERSION}" VERSION_LESS 3 AND "${CMAKE_MINOR_VERSION}" VERSION_LESS 1)
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

include ( GetPrerequisites )

# TODO: Make package name more configurable
# Android only uses shared libraries which are loaded, all else uses typical executables
# We do a test to check if a library is a shared library for Android
# For iOS, everything will be statically linked, which might be used for Android as well.
# For now, we leave the linking options here for desktop platforms
macro(COFFEE_ADD_EXAMPLE_LONG TARGET TITLE SOURCES LIBRARIES BUNDLE_LIBS BUNDLE_RSRCS)
    if(ANDROID)
        message( "Android Main: ${SDL2_ANDROID_MAIN_FILE}" )
        if(ANDROID_USE_SDL2_LAUNCH)
            add_library(${TARGET} SHARED ${SOURCES} "${SDL2_ANDROID_MAIN_FILE}" )
        else()
            message ("Android sources: ${SOURCES} ${ANDROID_GLUE_SOURCES} ${COFFEE_ANDROID_MAIN}")
            add_library(${TARGET} SHARED ${SOURCES} )

            target_link_libraries ( ${TARGET}
                AndroidCore
                )
        endif()
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    elseif(WIN32)
		WINPE_PACKAGE( ${TARGET} "${SOURCES}" "${BUNDLE_RSRCS}" )
    elseif(APPLE)
        set ( OSX_ICON "${CMAKE_SOURCE_DIR}/desktop/osx/Coffee.icns" )

        set_source_files_properties ( ${OSX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources" )

        set ( MACOSX_BUNDLE_LONG_VERSION_STRING ${COFFEE_BUILD_STRING} )
        set ( MACOSX_BUNDLE_BUNDLE_VERSION ${COFFEE_BUILD_STRING} )
        set ( MACOSX_BUNDLE_SHORT_VERSION_STRING ${COFFEE_VERSION_CODE} )
        set ( MACOSX_BUNDLE_ICON_FILE "Coffee.icns" )

        set ( MACOSX_BUNDLE_COPYRIGHT "Some of this code is under the MIT license" )
        set ( MACOSX_BUNDLE_BUNDLE_NAME ${TITLE} )
        set ( MACOSX_BUNDLE_INFO_STRING "Coffeecutie Game" )
        set ( MACOSX_BUNDLE_GUI_IDENTIFIER ${TITLE} )

        set ( BUNDLE_FILES )

        foreach(durr ${BUNDLE_RSRCS})
            file(GLOB_RECURSE TMP ${durr}/* )
            foreach(file ${TMP})
                file ( RELATIVE_PATH file_dir ${durr} ${file} )
                get_filename_component ( file_dir "${file_dir}" DIRECTORY )
                get_filename_component ( file_name "${file_dir}" NAME )
                if(NOT ("${file_name}" STREQUAL ".DS_Store"))
                    list ( APPEND BUNDLE_FILES ${file} )
                    set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${file_dir}" )
                endif()
            endforeach()
        endforeach()

        add_executable(${TARGET} MACOSX_BUNDLE ${BUNDLE_FILES} ${OSX_ICON} ${SOURCES})

        install(
            TARGETS
            ${TARGET}

            DESTINATION
            ${CMAKE_PACKAGED_OUTPUT_PREFIX}/osx
            )
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND COFFEE_GENERATE_APPIMAGE)
        add_executable( ${TARGET} ${SOURCES} )

        APPIMAGE_PACKAGE(${TARGET} "${TITLE}" "${BUNDLE_RSRCS}" "" "${BUNDLE_LIBS}")
    else()
        add_executable(${TARGET} ${SOURCES})

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

    if(APPLE)
    else()
    endif()

    if(ANDROID)

        # Lowercase the target name ofr package name
        string ( TOLOWER "${TARGET}" PACKAGE_SUFFIX )

        set ( DEPENDENCIES )

        list ( APPEND DEPENDENCIES $<TARGET_FILE:${TARGET}> )

        list ( APPEND DEPENDENCIES "${BUNDLE_LIBS}" )

        package_apk(
            "${TARGET}"
            "${TITLE}"
            "org.coffee.${PACKAGE_SUFFIX}"
            "${COFFEE_VERSION_CODE}" "${COFFEE_BUILD_STRING}"
            "${ANDROID_NATIVE_API_LEVEL}" "${ANDROID_ABI}"
            "${DEPENDENCIES}"
            ${BUNDLE_RSRCS}
            )

    endif()
endmacro()

macro(COFFEE_ADD_APPLICATION_LONG TARGET TITLE SOURCES LIBRARIES BUNDLE_LIBS BUNDLE_RSRCS)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "${BUNDLE}" "${RSRCS}")
endmacro()

macro(COFFEE_ADD_EXAMPLE TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()

macro(COFFEE_ADD_APPLICATION TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()

macro(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    add_executable ( ${TARGET} ${SOURCES} )

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
