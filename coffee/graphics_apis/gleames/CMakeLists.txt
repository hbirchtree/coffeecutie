find_package(SDL2 REQUIRED)

include_directories ( SYSTEM
        ${SDL2_INCLUDE_DIR}
    )

set ( CoffeeGLeamES_SRCS
                private/renderer/gleamrenderer.cpp
                private/levels/glbase.cpp

                private/glad/glad.c
    )

add_library( CoffeeGLeam ${COFFEE_LINK_OPT} ${CoffeeGLeamES_SRCS} )

target_link_libraries ( CoffeeGLeam
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
        )

set_property(TARGET CoffeeGLeam PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeGLeam)
