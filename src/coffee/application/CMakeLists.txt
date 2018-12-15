#
# Linked only by applications, contains main()
#
coffee_library (
    TARGET CoffeeCore_Application

    SOURCES
    private/coffee.cpp
    private/application_start.cpp

    LIBRARIES CoffeeCore_Base
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

if(ANDROID)
    find_package( AndroidGlue REQUIRED )

    target_sources ( CoffeeCore_Application PRIVATE
        platform/android/and_main.cpp
        platform/android/JNIHelper.cpp
        ${ANDROID_GLUE_SOURCES}
        )


    #
    # native_app_glue has a lot of spam in debug mode, we disable that
    #
    set_source_files_properties (
        ${ANDROID_GLUE_SOURCES}
        PROPERTIES
        COMPILE_DEFINITIONS "NDEBUG"
        )

    target_include_directories ( CoffeeCore_Application PUBLIC
        $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NDK_HELPER_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CPUFEATURES_INCLUDE_DIR}>
        )
    target_include_directories( CoffeeCore_Application PRIVATE
        platform/android/include
        )

endif()
