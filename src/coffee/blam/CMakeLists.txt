coffee_library (
    TARGET Blam
    SOURCES
    private/volta/cblam_map.cpp
    private/volta/cblam_mod2.cpp

    LIBRARIES Image CoreCompression

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

