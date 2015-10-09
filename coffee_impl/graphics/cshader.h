#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "cgraphicswrappers.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CGraphicsWrappers {

extern UseProgramStageMask operator~(UseProgramStageMask msk);

struct CPipeline
{
    GLuint handle = 0;
    UseProgramStageMask stages = GL_NONE_BIT;
};

struct CShaderProgram
{
    GLuint handle = 0;
    UseProgramStageMask stages = GL_NONE_BIT;
};

struct CShaderStageProgram
{
    GLuint handle = 0;
    UseProgramStageMask stage = GL_NONE_BIT;
};

struct CShader
{
    GLuint handle = 0;
    UseProgramStageMask stage = GL_NONE_BIT;
};

//For rendering
extern void coffee_graphics_bind(CPipeline* pl);
extern void coffee_graphics_unbind(CPipeline* pl);

//Resource management
extern void coffee_graphics_alloc(CPipeline* pl);
extern void coffee_graphics_alloc(CShaderProgram* shd, bool separable);

extern bool coffee_graphics_restore(
        CShaderProgram* prg,CResources::CResource* rsc);
extern void coffee_graphics_store(
        CShaderProgram* prg,CResources::CResource* rsc);

extern void coffee_graphics_free(CPipeline* pl);
extern void coffee_graphics_free(CShader* shd);
extern void coffee_graphics_free(CShaderProgram* prg);
extern void coffee_graphics_free(CShaderStageProgram* prg);

//Shader setup
extern bool coffee_graphics_shader_compile(CShader *prg, CResources::CResource *rsc, GLenum type, UseProgramStageMask stage);
extern bool coffee_graphics_shader_compile(CShaderStageProgram* prg, CResources::CResource *res, GLenum stage, UseProgramStageMask stageMask);

extern void coffee_graphics_shader_link(CShaderProgram* prg);

extern void coffee_graphics_shader_attach(
        CPipeline* pl, CShaderStageProgram* stg, UseProgramStageMask filter);
extern void coffee_graphics_shader_attach(
        CPipeline* pl, CShaderProgram* stg, UseProgramStageMask filter);

extern void coffee_graphics_shader_attach(CShaderProgram* shd, CShader* stg);
extern void coffee_graphics_shader_detach(
        CShaderProgram* shd, CShader* stg);

extern void coffee_graphics_shader_attach(
        CShaderProgram* shd, CShaderStageProgram* stg, UseProgramStageMask filter);

//Uniform blocks and values
extern void coffee_graphics_shader_uniform_block_get(CShaderProgram* prg, cglstring name, GLuint* loc);
extern void coffee_graphics_shader_uniform_block_set(
        CShaderProgram* prg, const CUniformBlock& block);

extern GLint coffee_graphics_shader_uniform_value_get(CShaderProgram* prg, cglstring name);

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
