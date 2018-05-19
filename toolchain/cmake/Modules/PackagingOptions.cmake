if(ANDROID)
    # The option to this is using the native_app_glue implementation which is not done
    option ( ANDROID_USE_SDL2_LAUNCH "Use traditional SDL2 Android bindings" ON )
    option ( ANDROID_DEPLOY_APK "Whether to install directly to a connected device" OFF )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    option ( COFFEE_GENERATE_APPIMAGE OFF )
    option ( COFFEE_GENERATE_FLATPAK  OFF )
    option ( COFFEE_GENERATE_SNAPPY   OFF )
endif()
