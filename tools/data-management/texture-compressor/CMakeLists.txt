find_package(bc7enc CONFIG REQUIRED)
find_package(Etc2Comp CONFIG REQUIRED)
find_package(Ktx CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

coffee_library(
  TARGET TextureCompression
  SOURCES 
    private/decoder.cpp
    private/encoder.cpp
    private/encoder_es.cpp
  LIBRARIES
    bc7enc::bc7enc
    Etc2Comp::EtcLib
    GraphicsDefines
    Image
    KTX::ktx
    magic_enum::magic_enum
    PeripheralPlatform
    Threads::Threads
    zstd::libzstd_static
)

coffee_application(
  TARGET TextureCompressor NO_COMPONENT_APP USE_CMD
  SOURCES private/compressor.cpp
  LIBRARIES CoreApplication TextureCompression
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_options(
    TextureCompressor PRIVATE -Wl,-rpath=../lib,-rpath=${CMAKE_BINARY_DIR}/lib
  )
endif()
