if(NOT COFFEE_BUILD_GLES)
    set ( GLeamBaseTest_SRCS
        main.cpp
        )

    include_directories(
        ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_OPENVR_INCLUDE_DIR}
        )

    add_executable ( GLeamBaseTest ${GLeamBaseTest_SRCS} )

    target_link_libraries ( GLeamBaseTest
        CoffeeCore
        CoffeeVR
        CoffeeGraphicsMisc
        CoffeeGLeam
        )

    target_compile_features(GLeamBaseTest PRIVATE cxx_variadic_templates)
endif()