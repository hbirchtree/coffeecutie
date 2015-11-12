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
        const _cbasic_graphics_shader_program* shader,
        _cbasic_graphics_resource_desc* uniform);

extern void coffee_graphics_uniform_set_texhandle(
        const _cbasic_graphics_shader_program* prog,
        const _cbasic_graphics_resource_desc* uniform,
        const CGuint64& value);

extern void coffee_graphics_uniform_set_texhandle_safe(
        const _cbasic_graphics_shader_program* prog,
        const _cbasic_graphics_resource_desc* uniform,
        const CGuint& value);

}
}

#endif // CUNIFORMTYPES

