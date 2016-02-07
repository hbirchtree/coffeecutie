set ( CoffeeGLeam_SRCS
                private/renderer/gleamrenderer.cpp
                private/levels/glbase.cpp

                private/glad/glad.c
    )

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" )

target_link_libraries ( CoffeeGLeam
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
        )