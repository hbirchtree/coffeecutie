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
        android_wrapperfun.cpp
        android_assets.cpp
        )
endif()

coffee_add_library ( AndroidCore
    "${AndroidCore_SRCS}" ""
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

# Suppress all those annoying double messages
set_source_files_properties (
    ${ANDROID_GLUE_SOURCES}
    PROPERTIES
    COMPILE_DEFINITIONS "NDEBUG"
    )

coffee_bundle_includes( ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries ( AndroidCore
    PRIVATE
    CoffeeCore
    )

target_include_directories ( AndroidCore
    PRIVATE
    ${ANDROID_GLUE_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

