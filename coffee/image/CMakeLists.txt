find_package ( stb REQUIRED )

set ( CoffeeImage_SRCS
        private/cimage.cpp
        private/cnoise-gen.cpp
        private/ctextrender.cpp
        )

coffee_add_library ( CoffeeImage "${CoffeeImage_SRCS}" ""
    "${CMAKE_SOURCE_DIR}/include/coffee/image")

target_include_directories ( CoffeeImage
    PRIVATE
    ${STB_INCLUDE_DIR}
    )
