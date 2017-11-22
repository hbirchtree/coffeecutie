set ( ANDROID_SOURCES_ANDROID ${ANDROID_NDK}/sources/android )

set ( ANDROID_GLUE_INCLUDE_DIR
    ${ANDROID_SOURCES_ANDROID}/native_app_glue
    CACHE PATH "Android Glue include directory" )
set ( ANDROID_GLUE_SOURCES
    ${ANDROID_SOURCES_ANDROID}/native_app_glue/android_native_app_glue.c
    CACHE FILEPATH "Android Glue source files" )

set ( NDK_HELPER_INCLUDE_DIR
    ${ANDROID_SOURCES_ANDROID}/ndk_helper
    CACHE PATH "NDK helper include directory" )
set ( NDK_HELPER_SOURCES
    "${ANDROID_SOURCES_ANDROID}/ndk_helper/gestureDetector.cpp;${ANDROID_SOURCES_ANDROID}/ndk_helper/JNIHelper.cpp"
    CACHE STRING "NDK helper source files" )

list ( APPEND ANDROID_GLUE_INCLUDE_DIR
    ${ANDROID_SOURCES_ANDROID}/cpufeatures )
list ( APPEND ANDROID_GLUE_SOURCES
    ${ANDROID_SOURCES_ANDROID}/cpufeatures/cpu-features.c )

if(ANDROID_GLUE_INCLUDE_DIR_TMP)
    set ( ANDROID_GLUE_INCLUDE_DIR
            ${ANDROID_GLUE_INCLUDE_DIR_TMP}
            ${CMAKE_SOURCE_DIR}/bindings/android/include )
endif()

if(ANDROID_GLUE_SOURCES_TMP)
    set ( ANDROID_GLUE_SOURCES ${ANDROID_GLUE_SOURCES_TMP} )
endif()

mark_as_advanced (
    ANDROID_GLUE_INCLUDE_DIR ANDROID_GLUE_SOURCES
    NDK_HELPER_INCLUDE_DIR NDK_HELPER_SOURCES
    )
