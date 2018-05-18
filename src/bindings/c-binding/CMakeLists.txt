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

if(COFFEE_NO_DYNAMIC_LINKING)
    # Shared libraries don't work on iOS
    set ( LINK_MODE STATIC )
endif()

coffee_library (
    TARGET  Cappuccino
    LINKAGE ${LINK_MODE}
    SOURCES ${Cappuccino_SRCS}
    LIBRARIES ${Cappuccino_LIBS}
    HEADER_DIRS ${Cappuccino_HDRS}
    )

target_include_directories ( Cappuccino
    PUBLIC
    $<BUILD_INTERFACE:${Cappuccino_HDRS}>
    $<INSTALL_INTERFACE:include/>
    )

coffee_bundle_includes ( ${Cappuccino_HDRS} )
