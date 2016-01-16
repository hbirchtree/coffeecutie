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

coffee_add_elibrary ( DynamicLib SHARED ${DynamicLib_SRCS} )
coffee_add_example ( DynamicTest ${DynamicTest_SRCS} )

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