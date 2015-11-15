#include "cuniformtypes.h"

#include "glbinding.h"
#include "cshader.h"

namespace Coffee{
namespace CGraphicsWrappers{

_cbasic_graphics_resource_desc::_cbasic_graphics_resource_desc():
    object_name(nullptr),
    index(0)
{
}

_cbasic_graphics_buffer_resource_desc::_cbasic_graphics_buffer_resource_desc():
    object_name(nullptr),
    buffer(nullptr),
    index(0)
{
}

void coffee_graphics_uniform_set_texhandle(
        const _cbasic_graphics_shader_program *prog,
        const _cbasic_graphics_resource_desc *uniform,
        const CGuint64 &value)
{
    glProgramUniformHandleui64ARB(prog->handle,
                                  uniform->index,
                                  value);
}

void coffee_graphics_uniform_set_texhandle_safe(
        const _cbasic_graphics_shader_program *prog,
        const _cbasic_graphics_resource_desc *uniform,
        const CGuint &value)
{
    glProgramUniform1i(prog->handle,
                       uniform->index,
                       value);
}

void coffee_graphics_uniform_get(
        const _cbasic_graphics_shader_program *shader,
        _cbasic_graphics_resource_desc *uniform)
{
    uniform->index = glGetUniformLocation(shader->handle,uniform->object_name);
}

}
}
