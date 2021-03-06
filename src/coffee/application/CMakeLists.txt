#
# Linked only by applications, contains main()
#
coffee_library (
    TARGET CoreApplication

    SOURCES
    private/coffee.cpp
    private/application_start.cpp

    LIBRARIES PeripheralPlatform Core CoreState ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_precompile_headers ( CoreApplication PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/core/CApplication>
    )

if(ANDROID OR (APPLE AND IOS))
    target_link_libraries ( CoreApplication PUBLIC
        ComponentBundleSetup
        )
endif()

if(ANDROID)
    set ( ANDROID_SOURCES_DIR "${ANDROID_NDK}/sources/android" )

    set ( APP_GLUE_SRC
        ${ANDROID_SOURCES_DIR}/native_app_glue/android_native_app_glue.c
        )

    target_sources ( CoreApplication PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/android/and_main.cpp
        ${APP_GLUE_SRC}
        )
    target_include_directories( CoreApplication PRIVATE
        platform/android/include
        )

    get_source_file_property (
        PREV_COMPILE_DEFINITIONS ${APP_GLUE_SRC} COMPILE_DEFINITIONS
        )
    set_source_files_properties ( ${APP_GLUE_SRC}
        PROPERTIES
        COMPILE_DEFINITIONS "${PREV_COMPILE_DEFINITIONS};NDEBUG=1"
        )
elseif(APPLE AND IOS)
    option ( IOS_GLKIT_APP "Build iOS with GLKit for OpenGLES support" ON )

    target_sources ( CoreApplication PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/uikit/AppDelegate.mm>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/uikit/main.m>
        )
    target_compile_definitions ( CoreApplication PRIVATE
        -DGLES_SILENCE_DEPRECATION
        )

    if(IOS_GLKIT_APP)
        target_compile_definitions ( CoreApplication PRIVATE
            -DCOFFEE_APP_USE_GLKIT
            )
        target_link_libraries ( CoreApplication PUBLIC
            CEAGL
            )
    endif()
endif()

if(GAMECUBE)
    target_link_libraries ( CoreApplication PUBLIC
        Gexxo # For initializing video
        )
endif()
