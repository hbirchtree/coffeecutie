set ( LINK_MODE SHARED )

if(NO_DYNAMIC_LINKING)
    set ( LINK_MODE STATIC )
endif()

coffee_library (
    TARGET  Cappuccino
    LINKAGE ${LINK_MODE}

    SOURCES
    private/cappuccino.cpp

    LIBRARIES
    CoffeeCore

    HEADER_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_include_directories ( Cappuccino
    PUBLIC
    $<BUILD_INTERFACE:${Cappuccino_HDRS}>
    $<INSTALL_INTERFACE:include/>
    )

coffee_bundle_includes ( ${CMAKE_CURRENT_SOURCE_DIR}/include )
