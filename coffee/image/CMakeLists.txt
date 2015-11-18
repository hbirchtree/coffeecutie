if(NOT DEFINED GLBINDING_INCLUDE_DIR)
    set (GLBINDING_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/glbinding/source/glbinding/include)
endif()

include_directories (
                    ${GLBINDING_INCLUDE_DIR}
                    )

set ( CImage_SRCS
        cimage.cpp
        )

add_library( CImage ${COFFEE_LINK_OPT} ${CImage_SRCS})

target_link_libraries( CImage
    CoffeeCore
    )

target_compile_features(CImage PRIVATE cxx_variadic_templates)
