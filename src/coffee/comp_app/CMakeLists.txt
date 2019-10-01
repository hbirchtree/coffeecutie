coffee_library (
    TARGET ComponentApp
    SOURCES
    private/subsystems.cpp
    private/comp_app.cpp

    LIBRARIES
    EntComp
    Core

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

coffee_library (
    TARGET ComponentBundleSetup
    SOURCES private/bundle.cpp
    LIBRARIES ComponentApp ComponentBundle
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bundle_include>
    $<INSTALL_INTERFACE:include>
    )

coffee_library (
    TARGET DropinRenderer
    SOURCES private/dropin.cpp
    LIBRARIES ComponentBundleSetup
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dropin_include>
    $<INSTALL_INTERFACE:include>
    )

coffee_bundle (
    HEADER_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bundle_include/coffee
    ${CMAKE_CURRENT_SOURCE_DIR}/dropin_include/coffee
    )
