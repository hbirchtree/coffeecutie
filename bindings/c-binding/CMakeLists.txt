set ( Cappuccino_SRCS
    private/cappuccino.cpp
    )
set ( Cappuccino_LIBS
    CoffeeCore
    )
set ( Cappuccino_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

include_directories (
    ${Cappuccino_HDRS}
    )

coffee_add_elibrary (
    Cappuccino SHARED
    "${Cappuccino_SRCS}"
    "${Cappuccino_LIBS}"
    "${Cappuccino_HDRS}"
    )
