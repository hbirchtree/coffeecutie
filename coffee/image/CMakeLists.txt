include_directories (
        ${COFFEE_IMAGE_INCLUDE_DIR}
    )

set ( CoffeeImage_SRCS
        private/cimage.cpp
        )

add_library( CoffeeImage ${COFFEE_LINK_OPT} ${CoffeeImage_SRCS})

target_link_libraries( CoffeeImage
        CoffeeCore
    )

set_property(TARGET CoffeeImage PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeImage)
