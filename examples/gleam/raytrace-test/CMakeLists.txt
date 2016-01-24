set ( RaytraceTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    ${COFFEE_IMAGE_INCLUDE_DIR}
    )

coffee_add_example ( RaytraceTest ${RaytraceTest_SRCS} )

target_link_libraries ( RaytraceTest
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeImage
    CoffeeGLeam
    )
