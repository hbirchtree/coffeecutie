set ( CoffeeGLeam_SRCS
                private/renderer/gleamrenderer.cpp
                private/levels/glbase.cpp

                private/glad/glad.c
    )

if(COFFEE_BUILD_GLES)
    add_definitions(-DCOFFEE_ESMODE)
endif()

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" )

target_link_libraries ( CoffeeGLeam
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
        )