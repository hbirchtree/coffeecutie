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
