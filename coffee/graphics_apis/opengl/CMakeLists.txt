find_package(glbinding)

set ( CoffeeOpenGL_SRCS
        private/cgraphics_quirks.cpp
        private/unit_tests.cpp

        private/glbinding/cglbindingrenderer.cpp
        private/glbinding/glbinding.cpp

        private/wrappers/cbuffer.cpp
        private/wrappers/cbuffer_storage.cpp
        private/wrappers/cbuffer_storage_fallback.cpp
        private/wrappers/cshader.cpp
        private/wrappers/cframebuffer.cpp
        private/wrappers/cgraphicswrappers.cpp
        private/wrappers/cvertexarrayobject.cpp
        private/wrappers/ctexture.cpp
        private/wrappers/ctexture_dxtc.cpp
        private/wrappers/crenderfence.cpp
        private/wrappers/copengl_types.cpp
        private/wrappers/ctexture_storage.cpp
        private/wrappers/ctexture_storage_fallback.cpp
        private/wrappers/ctexture_sampler.cpp
        private/wrappers/ctexture_pbo.cpp
        private/wrappers/cuniformtypes.cpp
        private/wrappers/cviewport_array.cpp
    )

add_library( CoffeeOpenGL ${COFFEE_LINK_OPT} ${CoffeeOpenGL_SRCS} )

include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${GLBINDING_INCLUDE_DIRS}
    )

target_link_libraries ( CoffeeOpenGL
                CoffeeCore
                CoffeeImage
                ${GLBINDING_LIBRARIES}
        )

set_property(TARGET CoffeeOpenGL PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeOpenGL PRIVATE cxx_variadic_templates)
