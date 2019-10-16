#
# Linked only by applications, contains main()
#
coffee_library (
    TARGET CoreApplication

    SOURCES
    private/coffee.cpp
    private/application_start.cpp

    LIBRARIES Core CoreState ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

if(ANDROID OR (APPLE AND IOS))
    target_link_libraries ( CoreApplication PUBLIC
        ComponentBundleSetup
        )
endif()

if(ANDROID)
    find_package( AndroidGlue REQUIRED )

    target_sources ( CoreApplication PRIVATE
        platform/android/JNIHelper.cpp
        )
    target_sources ( CoreApplication PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/android/and_main.cpp>
        )

    target_include_directories ( CoreApplication PUBLIC
        $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NDK_HELPER_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CPUFEATURES_INCLUDE_DIR}>
        )
    target_include_directories( CoreApplication PRIVATE
        platform/android/include
        )
elseif(APPLE AND IOS)
    option ( IOS_GLKIT_APP "Build iOS with GLKit for OpenGLES support" ON )

    target_sources ( CoreApplication PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/uikit/AppDelegate.mm>
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
