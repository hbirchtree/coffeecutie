#include <wrappers/cuniformtypes.h>

#include <glfunctions.h>
#include <wrappers/cshader.h>

namespace Coffee{
namespace CGraphicsWrappers{

_cbasic_graphics_resource_desc::_cbasic_graphics_resource_desc():
    object_name(nullptr),
    index(0)
{
}

void coffee_graphics_uniform_set_texhandle(const _cbasic_graphics_shader_program &prog,
        const CUniform &uniform,
        const CGuint64 &value)
{
    glProgramUniformHandleui64ARB(prog.handle,
                                  uniform.index,
                                  value);
}

void coffee_graphics_uniform_set_texhandle_safe(
        const _cbasic_graphics_shader_program &prog,
        const CUniform &uniform,
        const CGuint &value)
{
    glProgramUniform1i(prog.handle,
                       uniform.index,
                       value);
}

void coffee_graphics_uniform_get(
        const _cbasic_graphics_shader_program &shader,
        CUniform &uniform)
{
    uniform.index = glGetUniformLocation(
                shader.handle,
                uniform.object_name);
}

void coffee_graphics_subroutine_get(const _cbasic_graphics_shader_program &shader,
        CShaderSubroutine &routine)
{
    routine.index = glGetSubroutineIndex(
                shader.handle,
                gl_get(shader.stage),
                routine.object_name);
}

void coffee_graphics_uniform_subroutine_get(
        const _cbasic_graphics_shader_program &shader,
        CUniformSubroutine &routine)
{
    routine.index = glGetSubroutineUniformLocation(
                shader.handle,
                gl_get(shader.stage),
                routine.object_name);
}

void coffee_graphics_uniform_block_get(
        const _cbasic_graphics_shader_program &shader,
        CUniformBlock &uBlock)
{
    uBlock.index = glGetUniformBlockIndex(shader.handle,uBlock.object_name);
}

void coffee_graphics_uniform_block_set(
        _cbasic_graphics_shader_program &shader,
        const CUniformBlockBinding &block)
{
    glUniformBlockBinding(shader.handle,
                          block.desc->index,
                          block.index);
}

void coffee_graphics_uniform_subroutine_set(
        _cbasic_graphics_shader_program &shader,
        const CShaderSubroutine &routine,
        const CUniformSubroutine &routineUniform)
{
    C_UNUSED(shader,routine,routineUniform);
    cWarning("Still nothing here!");
}

void coffee_graphics_shaderstoragebuffer_get(const _cbasic_graphics_shader_program &shader,
        CShaderStorageBuffer &sBuffer)
{
    sBuffer.index = glGetProgramResourceIndex(
                shader.handle,GL_SHADER_STORAGE_BLOCK,sBuffer.object_name);
}

void coffee_graphics_shaderstoragebuffer_set(
        _cbasic_graphics_shader_program &prg,
        const CShaderStorageBufferBinding &block)
{
    glShaderStorageBlockBinding(prg.handle,
                                block.desc->index,
                                block.index);
}

}
}
