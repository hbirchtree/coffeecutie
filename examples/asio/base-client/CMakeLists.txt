set ( AsioClientTest_SRCS
        main.cpp
)

add_definitions("-DASIO_STANDALONE")

add_executable ( AsioClientTest ${AsioClientTest_SRCS} )

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    )

target_link_libraries ( AsioClientTest
        CoffeeCore
        )

target_compile_features(AsioClientTest PRIVATE cxx_variadic_templates)
