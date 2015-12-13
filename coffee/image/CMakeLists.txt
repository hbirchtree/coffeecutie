include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set ( CImage_SRCS
        private/cimage.cpp
        )

add_library( CImage ${COFFEE_LINK_OPT} ${CImage_SRCS})

target_link_libraries( CImage
        CoffeeCore
    )

target_compile_features(CImage PRIVATE cxx_variadic_templates)
