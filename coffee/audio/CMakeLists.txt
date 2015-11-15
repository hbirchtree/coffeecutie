
set ( CAudio_SRCS
        libs/stb/stb_vorbis.c
        coffee/audio/caudio.cpp
        coffee/audio/openal/copenal.cpp
        )

add_library( CAudio ${COFFEE_LINK_OPT} ${CAudio_SRCS})

find_package(OpenAL REQUIRED)
target_include_directories(CAudio SYSTEM PRIVATE "${OPENAL_INCLUDE_DIR}")

target_link_libraries ( CAudio
${OPENAL_LIBRARY}
)

target_compile_features(CAudio PRIVATE cxx_variadic_templates)
