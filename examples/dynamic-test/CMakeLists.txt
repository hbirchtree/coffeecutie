set ( DynamicTest_SRCS
        main.cpp
    )

set (DynamicLib_SRCS
        dynatest.cpp
    )

include_directories (
        ${COFFEE_CORE_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
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
        CImage
        CAudio
    )

target_compile_features(DynamicTest PRIVATE cxx_variadic_templates)
target_compile_features(DynamicLib PRIVATE cxx_variadic_templates)
