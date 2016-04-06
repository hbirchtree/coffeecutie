set ( CoffeeGLeam_SRCS
    private/renderer/gleamrenderer.cpp
    private/levels/glbase.cpp

    private/glad/glad.c
    )

if(COFFEE_BUILD_GLES)
    add_definitions(-DCOFFEE_ESMODE)
    list(APPEND CoffeeGLeam_SRCS private/glad_es/glad.c)
else()
    list(APPEND CoffeeGLeam_SRCS private/glad/glad.c)
endif()

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" )

target_link_libraries ( CoffeeGLeam
    CoffeeCore
    CoffeeImage
    CoffeeSDL2
    )

# For Android, there is some special function loading
    find_package(SDL2 REQUIRED)
    include_directories (${SDL2_INCLUDE_DIR})
    target_link_libraries(CoffeeGLeam ${SDL2_LIBRARY})
