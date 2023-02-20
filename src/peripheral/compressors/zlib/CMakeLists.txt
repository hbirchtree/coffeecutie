if(NOT EMSCRIPTEN AND NOT APPLE)
    find_package ( ZLIB )
endif()

coffee_library (
    TARGET CoreZ

    SOURCES
    private/corez.cpp

    LIBRARIES PeripheralLib

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    HEADER_BASE corez
    )

if(TARGET ZLIB::ZLIB AND NOT EMSCRIPTEN AND NOT APPLE)
    target_link_libraries ( CoreZ PUBLIC
        ZLIB::ZLIB
        )
elseif(EMSCRIPTEN)
    target_compile_options ( CoreZ PUBLIC
        -sUSE_ZLIB=1
       )
   target_link_options ( CoreZ PUBLIC
        -sUSE_ZLIB=1
       )
elseif(ANDROID OR APPLE)
    target_link_libraries ( CoreZ PUBLIC
        z
        )
else()
    target_sources ( CoreZ PRIVATE
        impl/inflate.c
        impl/deflate.c
        impl/infback.c
        impl/inffast.c
        impl/inftrees.c
        impl/trees.c
        impl/adler32.c
        impl/crc32.c
        impl/zutil.c
        )
    target_include_directories ( CoreZ PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/include>
        )
endif()
