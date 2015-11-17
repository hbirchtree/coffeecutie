
set ( CAudio_SRCS
        ${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c
        caudio.cpp
        openal/copenal.cpp
        )

add_library( CAudio ${COFFEE_LINK_OPT} ${CAudio_SRCS})

find_package(OpenAL REQUIRED)
include_directories(
    ${OPENAL_INCLUDE_DIR}
    )

target_link_libraries ( CAudio
    ${OPENAL_LIBRARY}
    )

target_compile_features(CAudio PRIVATE cxx_variadic_templates)
