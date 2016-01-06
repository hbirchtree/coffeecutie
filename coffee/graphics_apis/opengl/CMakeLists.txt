find_package(glbinding)

set ( CoffeeOpenGL_SRCS
        private/cgraphics_quirks.cpp
        private/unit_tests.cpp

        private/glbinding/cglbindingrenderer.cpp
        private/glbinding/glbinding.cpp

        private/glbinding/wrappers/cbuffer.cpp
        private/glbinding/wrappers/cbuffer_storage.cpp
        private/glbinding/wrappers/cbuffer_storage_fallback.cpp
        private/glbinding/wrappers/cshader.cpp
        private/glbinding/wrappers/cframebuffer.cpp
        private/glbinding/wrappers/cgraphicswrappers.cpp
        private/glbinding/wrappers/cvertexarrayobject.cpp
        private/glbinding/wrappers/ctexture.cpp
        private/glbinding/wrappers/ctexture_dxtc.cpp
        private/glbinding/wrappers/crenderfence.cpp
        private/glbinding/wrappers/copengl_types.cpp
        private/glbinding/wrappers/ctexture_storage.cpp
        private/glbinding/wrappers/ctexture_storage_fallback.cpp
        private/glbinding/wrappers/ctexture_sampler.cpp
        private/glbinding/wrappers/ctexture_pbo.cpp
        private/glbinding/wrappers/cuniformtypes.cpp
        private/glbinding/wrappers/cviewport_array.cpp
    )

include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_SDL2_INCLUDE_DIR}
        ${GLBINDING_INCLUDE_DIRS}
    )

add_library( CoffeeOpenGL ${COFFEE_LINK_OPT} ${CoffeeOpenGL_SRCS} )


target_link_libraries ( CoffeeOpenGL
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
                ${GLBINDING_LIBRARIES}
        )

set_property(TARGET CoffeeOpenGL PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeOpenGL PRIVATE cxx_variadic_templates)
