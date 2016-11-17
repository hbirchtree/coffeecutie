set ( CoffeeImage_SRCS
        private/cimage.cpp
        private/cnoise-gen.cpp
        private/ctextrender.cpp
        )

coffee_add_library ( CoffeeImage "${CoffeeImage_SRCS}" "CoffeeCore" )
