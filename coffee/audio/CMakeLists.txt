set ( CAudio_SRCS
        ${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c
        caudio.cpp
        openal/copenal.cpp
        )

add_library( CAudio ${COFFEE_LINK_OPT} ${CAudio_SRCS})

target_link_libraries ( CAudio
    CoffeeCore
    ${OPENAL_LIBRARY}
    )

target_compile_features(CAudio PRIVATE cxx_variadic_templates)
