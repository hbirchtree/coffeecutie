#
# Linked only by applications, contains main()
#
coffee_library (
    TARGET CoreApplication

    SOURCES
    private/coffee.cpp
    private/application_start.cpp

    LIBRARIES CoreBase
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

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

endif()
