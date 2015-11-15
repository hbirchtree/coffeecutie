set ( CImage_SRCS
        coffee/image/cimage.cpp
        )

add_library( CImage ${COFFEE_LINK_OPT} ${CImage_SRCS})

target_compile_features(CImage PRIVATE cxx_variadic_templates)
