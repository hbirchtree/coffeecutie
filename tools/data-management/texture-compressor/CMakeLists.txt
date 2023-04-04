#find_package ( bc7enc CONFIG REQUIRED )
find_package ( Etc2Comp CONFIG REQUIRED )
find_package ( Ktx CONFIG REQUIRED )
find_package (Threads REQUIRED )
find_package (zstd CONFIG REQUIRED )

coffee_library(
    TARGET TextureCompression
    NO_COMPONENT_APP
    SOURCES
        private/decoder.cpp
        private/encoder.cpp
    LIBRARIES
        Image
        PeripheralPlatform
        Etc2Comp::EtcLib
        KTX::ktx
        Threads::Threads
        zstd::libzstd_static
    )

coffee_application(
    TARGET TextureCompressor
    SOURCES
        private/compressor.cpp
    LIBRARIES
        CoreApplication
        TextureCompression
    )

target_link_options ( TextureCompressor PRIVATE
    -Wl,-rpath=../lib,-rpath=${CMAKE_BINARY_DIR}/lib
    )
