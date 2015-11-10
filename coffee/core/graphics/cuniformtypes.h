#ifndef CUNIFORMTYPES
#define CUNIFORMTYPES

#include "coffee/core/coffee.h"
#include "copengl_types.h"
#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

typedef _cbasic_graphics_resource_desc CUniform;
typedef _cbasic_graphics_buffer_resource_desc CUniformBlock;
typedef _cbasic_graphics_buffer_resource_desc CStorageBlock;

typedef _cbasic_graphics_resource_binding<CUniform> CUniformBinding;
typedef _cbasic_graphics_resource_binding<CUniformBlock> CUniformBlockBinding;
typedef _cbasic_graphics_resource_binding<CStorageBlock> CStorageBlockBinding;

extern void coffee_graphics_uniform_get(
        CUniform* unif,
        _cbasic_graphics_shader_program* shader);

}
}

#endif // CUNIFORMTYPES

