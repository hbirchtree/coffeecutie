include ( AndroidToolkit )
include ( AndroidApkBuild )

# Wrappers to get rid of boilerplate and cross-platform-ness (ahem, Android)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID)
        #Android's compiler doesn't support target_compile_features :(
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
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
        add_library(${TARGET} SHARED ${ANDROID_SDL_MAIN_UNIT} ${SOURCES} )
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    else()
        add_executable(${TARGET} ${SOURCES})
    endif()
    target_enable_cxx11(${TARGET})

    target_link_libraries ( ${TARGET}
        ${LIBRARIES}
        )

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        bin
        )

    if(ANDROID)

        # Lowercase the target name ofr package name
        string ( TOLOWER "${TARGET}" PACKAGE_SUFFIX )

        set ( DEPENDENCIES )

        list ( APPEND DEPENDENCIES "$<TARGET_FILE:${TARGET}>" )

        list ( APPEND DEPENDENCIES "${ARGN}" )

        package_apk(
            "${TARGET}"
            "${TITLE}"
            "org.coffee.${PACKAGE_SUFFIX}"
            "1" "1.0"
            "19" "armeabi-v7a"
            "${DEPENDENCIES}" )

#        package_apk(
#            "${TARGET}"
#            "${TITLE}"
#            "org.coffee.${PACKAGE_SUFFIX}"
#            "1" "1.0"
#            "21" "arm64-v8a"
#            "${DEPENDENCIES}" )

    endif()
endmacro()
