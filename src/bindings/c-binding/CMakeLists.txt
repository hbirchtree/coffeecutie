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
    Core

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

coffee_bundle_includes ( ${CMAKE_CURRENT_SOURCE_DIR}/include )
