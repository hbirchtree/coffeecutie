find_package ( Stb REQUIRED )
find_package ( Ktx CONFIG REQUIRED )
find_package ( Threads REQUIRED )
find_package ( zstd CONFIG REQUIRED )

coffee_library (
    TARGET Image

    LIBRARIES
        Core
        KTX::ktx
        Threads::Threads
        zstd::libzstd_static

    SOURCES
        private/cimage.cpp
        # private/ctextrender.cpp
        # private/cnoise-gen.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( Image SYSTEM PRIVATE
    $<BUILD_INTERFACE:${Stb_INCLUDE_DIR}>
    )

if(TARGET GraphicsDefines)
    target_sources( Image PRIVATE
        private/ktx_load.cpp
        )
    target_link_libraries ( Image PRIVATE
        GraphicsDefines
        )
endif()
