find_package(OpenAL REQUIRED)

include_directories(
        ${OPENAL_INCLUDE_DIR}
        ${COFFEE_CORE_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set ( CoffeeAudio_SRCS
        ${COFFEE_EXT_LIBRARY_DIR}/stb/stb_vorbis.c
        private/caudio.cpp
        private/openal/copenal.cpp
        )

add_library( CoffeeAudio ${COFFEE_LINK_OPT} ${CoffeeAudio_SRCS})

target_link_libraries ( CoffeeAudio
    CoffeeCore
    ${OPENAL_LIBRARY}
    )

set_property(TARGET CoffeeAudio PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeAudio PRIVATE cxx_variadic_templates)
