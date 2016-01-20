include_directories (
        ${COFFEE_IMAGE_INCLUDE_DIR}
    )

set ( CoffeeImage_SRCS
        private/cimage.cpp
        private/cnoise-gen.cpp
        )

coffee_add_library ( CoffeeImage "${CoffeeImage_SRCS}" )

target_link_libraries( CoffeeImage
        CoffeeCore
    )