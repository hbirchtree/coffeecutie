coffee_library (
    TARGET Blam
    SOURCES
    private/volta/cblam_map.cpp
    private/volta/cblam_mod2.cpp

    include/blam/cblam.h

    LIBRARIES Image CoreCompression

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_precompile_headers ( Blam PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:blam/cblam.h>
    )
