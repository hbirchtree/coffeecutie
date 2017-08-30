# This is where we put the Android-specific shim functions (JNI and etc.)

find_package ( AndroidGlue )

if(ANDROID_USE_SDL2_LAUNCH)
    find_package ( SDL2main )
    find_package ( SDL2 )
endif()

set ( AndroidCore_SRCS
    stub.cpp
    #        static.cpp
    )

if(NOT ANDROID_USE_SDL2_LAUNCH)
    list (APPEND AndroidCore_SRCS
        ${ANDROID_GLUE_SOURCES} ${COFFEE_ANDROID_MAIN}
        #            ${SDL2_ANDROID_MAIN_FILE}
        )
endif()

coffee_add_library ( AndroidCore
    "${AndroidCore_SRCS}" ""
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

coffee_bundle_includes( ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_include_directories ( AndroidCore
    PRIVATE
    $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
