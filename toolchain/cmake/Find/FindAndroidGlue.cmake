set ( ANDROID_SOURCES_ANDROID ${ANDROID_NDK}/sources/android )

set ( ANDROID_GLUE_INCLUDE_DIR
    "${ANDROID_SOURCES_ANDROID}/native_app_glue"
    CACHE PATH "Android Glue include directory" )
set ( ANDROID_GLUE_SOURCES
    "${ANDROID_SOURCES_ANDROID}/native_app_glue/android_native_app_glue.c"
    CACHE FILEPATH "Android Glue source files" )

set ( NDK_HELPER_INCLUDE_DIR
    ${ANDROID_SOURCES_ANDROID}/ndk_helper
    CACHE PATH "NDK helper include directory" )
set ( NDK_HELPER_SOURCES
    "${ANDROID_SOURCES_ANDROID}/ndk_helper/gestureDetector.cpp"
    CACHE STRING "NDK helper source files" )

set ( CPUFEATURES_INCLUDE_DIR
    "${ANDROID_SOURCES_ANDROID}/cpufeatures"
    CACHE PATH "CPU features include directory" )
set ( CPUFEATURES_SOURCES
    "${ANDROID_SOURCES_ANDROID}/cpufeatures/cpu-features.c"
    CACHE STRING "CPU features source files" )

if(NOT TARGET Glue AND NOT TARGET GlueCpuFeatures AND NOT TARGET GlueNdkHelper)
    add_library ( Glue INTERFACE )
    add_library ( GlueCpuFeatures INTERFACE )
    add_library ( GlueNdkHelper INTERFACE )

    target_sources ( GlueCpuFeatures INTERFACE
        ${CPUFEATURES_SOURCES}
        )

    target_sources ( Glue INTERFACE
        ${ANDROID_GLUE_SOURCES}
        )

    target_sources ( GlueNdkHelper INTERFACE
        ${NDK_HELPER_SOURCES}
        )

    target_include_directories ( GlueCpuFeatures INTERFACE
        ${CPUFEATURES_INCLUDE_DIR}
        )

    target_include_directories ( Glue INTERFACE
        ${ANDROID_GLUE_INCLUDE_DIR}
        )

    target_include_directories ( GlueNdkHelper INTERFACE
        ${NDK_HELPER_INCLUDE_DIR}
        )

    #
    # native_app_glue has a lot of spam in debug mode, we disable that
    #
    set_source_files_properties (
        ${ANDROID_GLUE_SOURCES}
        PROPERTIES
        COMPILE_DEFINITIONS "NDEBUG"
        )
endif()
