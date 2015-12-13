find_package(glbinding)

set ( CoffeeOpenGL_SRCS

        glbinding/cglbindingrenderer.cpp
        glbinding/glbinding.cpp

        cgraphics_quirks.cpp

        wrappers/cbuffer.cpp
        wrappers/cbuffer_storage.cpp
        wrappers/cbuffer_storage_fallback.cpp
        wrappers/cshader.cpp
        wrappers/cframebuffer.cpp
        wrappers/cgraphicswrappers.cpp
        wrappers/cvertexarrayobject.cpp
        wrappers/ctexture.cpp
        wrappers/ctexture_dxtc.cpp
        wrappers/crenderfence.cpp
        wrappers/copengl_types.cpp
        wrappers/ctexture_storage.cpp
        wrappers/ctexture_storage_fallback.cpp
        wrappers/ctexture_sampler.cpp
        wrappers/ctexture_pbo.cpp
        wrappers/cuniformtypes.cpp
        wrappers/cviewport_array.cpp
        )

add_library( CoffeeOpenGL ${COFFEE_LINK_OPT} ${CoffeeOpenGL_SRCS} )

include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${GLBINDING_INCLUDE_DIRS})

target_link_libraries ( CoffeeOpenGL
                CoffeeCore
                CImage
                ${GLBINDING_LIBRARIES}
        )

target_compile_features(CoffeeOpenGL PRIVATE cxx_variadic_templates)
