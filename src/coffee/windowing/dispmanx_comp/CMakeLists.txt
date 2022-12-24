coffee_library (
    TARGET DispManXComponent
    SOURCES private/dispmanx_comp.cpp
    LIBRARIES
        ComponentApp
        khrn_static
        vchiq_arm
        vchostif
        vcos
        bcm_host
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
