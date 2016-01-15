set ( DynamicTest_SRCS
    main.cpp
    )

set (DynamicLib_SRCS
    dynatest.cpp
    )

include_directories (
    ${COFFEE_CORE_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    ${COFFEE_AUDIO_INCLUDE_DIR}
    ${COFFEE_IMAGE_INCLUDE_DIR}
    )

add_library ( DynamicLib SHARED ${DynamicLib_SRCS} )

set_property(TARGET DynamicLib PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable ( DynamicTest ${DynamicTest_SRCS} )

target_link_libraries ( DynamicTest
    CoffeeCore
    )
target_link_libraries ( DynamicLib
    CoffeeCore
    CoffeeOpenGL
    CoffeeGraphicsMisc
    CoffeeImage
    CoffeeAudio
    )

target_enable_cxx11(DynamicTest)
target_enable_cxx11(DynamicLib)
