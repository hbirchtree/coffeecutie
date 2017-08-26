set ( Cappuccino_SRCS
    private/cappuccino.cpp
    )
set ( Cappuccino_LIBS
    CoffeeCore
    )
set ( Cappuccino_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

set ( LINK_MODE SHARED )

if(APPLE AND IOS)
    # Shared libraries don't work on iOS
    set ( LINK_MODE STATIC )
endif()

coffee_add_elibrary (
    Cappuccino ${LINK_MODE}
    "${Cappuccino_SRCS}"
    "${Cappuccino_LIBS}"
    "${Cappuccino_HDRS}"
    )

target_include_directories ( Cappuccino
    PUBLIC
    ${Cappuccino_HDRS}
    )
