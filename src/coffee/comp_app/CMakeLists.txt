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
    )

target_precompile_headers ( ComponentApp PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/subsystems.h>
    )

coffee_library (
    TARGET ComponentBundleSetup
    SOURCES private/bundle.cpp
    LIBRARIES ComponentApp ComponentBundle GraphicsBundle MobileBundle Image
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bundle_include>
    )

target_precompile_headers ( ComponentBundleSetup PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/bundle.h>
    $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/app_wrap.h>
    $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/AppWrap>
    )

coffee_library (
    TARGET DropinRenderer
    SOURCES private/dropin_stub.cpp
    LIBRARIES ComponentBundleSetup
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dropin_include>
    )
