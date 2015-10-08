#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "cgraphicswrappers.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CGraphicsWrappers {

struct CShaderStageProgram;
struct CShaderProgram;
struct CShader;

struct CPipeline
{
    GLuint handle = 0;
    UseProgramStageMask stages;

    void bind();
    void unbind();

    bool create();
    void free();
    void attachStages(CShaderStageProgram *shader, UseProgramStageMask bits);
    void attachProgram(CShaderProgram *shader, UseProgramStageMask bits);
};

struct CShaderProgram
{
    GLuint handle = 0;
    UseProgramStageMask stages;

    void create(bool separable = true);
    void attachShader(CShader* shader, UseProgramStageMask maskopt);
    void detachShader(CShader* shader);
    void link();
    void free();

    void storeProgram(CResources::CResource* out);
    bool fetchProgram(CResources::CResource* in);

    GLint uniformLocation(cstring name);

    void uniformBlockIndex(CUniformBlock *block);
    void setUniformBlockBind(const CUniformBlock &block);
};

struct CShaderStageProgram
{
    GLuint handle = 0;
    UseProgramStageMask stage;

    bool compile(CResources::CResource* res, GLenum stage, UseProgramStageMask stageMask);
    void free();
};

struct CShader
{
    GLuint handle = 0;
    GLenum stage = GL_NONE;

    bool compile(CResources::CResource* res, GLenum stage);
    void free();
};

//For rendering
extern void coffee_graphics_bind(CPipeline* pl);
extern void coffee_graphics_unbind(CPipeline* pl);

//Resource management
extern void coffee_graphics_alloc(CPipeline* pl);
extern void coffee_graphics_alloc(CShader* shd);
extern void coffee_graphics_alloc(CShaderProgram* prg);
extern void coffee_graphics_alloc(CShaderStageProgram* prg);

extern void coffee_graphics_restore(
        CShaderProgram* prg,CResources::CResource* rsc);
extern void coffee_graphics_store(
        CShaderProgram* prg,CResources::CResource* rsc);

extern void coffee_graphics_free(CPipeline* pl);
extern void coffee_graphics_free(CShader* shd);
extern void coffee_graphics_free(CShaderProgram* prg);
extern void coffee_graphics_free(CShaderStageProgram* prg);

//Shader setup
extern void coffee_graphics_compile(CShaderProgram* prg);
extern void coffee_graphics_compile(CShaderStageProgram* prg);

extern void coffee_graphics_link(CShader* shd);
extern void coffee_graphics_link(CShaderProgram* prg);

extern void coffee_graphics_attach(
        CPipeline* pl, CShaderStageProgram* stg, UseProgramStageMask filter);
extern void coffee_graphics_attach(
        CPipeline* pl, CShaderProgram* stg, UseProgramStageMask filter);

extern void coffee_graphics_attach(
        CShaderProgram* shd, CShader* stg, UseProgramStageMask filter);
extern void coffee_graphics_detach(
        CShaderProgram* shd, CShader* stg);

extern void coffee_graphics_attach(
        CShaderProgram* shd, CShaderStageProgram* stg, UseProgramStageMask filter);

//Uniform blocks and values
extern void coffee_graphics_uniform_block_get(CShaderProgram* prg, cstring name);
extern void coffee_graphics_uniform_block_set(
        CShaderProgram* prg, const CUniformBlock& block);

extern void coffee_graphics_uniform_value_get(CShaderProgram* prg, cstring name);

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
