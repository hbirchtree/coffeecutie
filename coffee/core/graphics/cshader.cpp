#include "cshader.h"

namespace Coffee {
namespace CGraphicsWrappers {

bool coffee_shader_compile_checklog(GLuint handle);
bool coffee_program_link_checklog(GLuint handle);

bool coffee_shader_compile_checklog(GLuint handle){
    GLint succ = 0;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&succ);
    if(!succ){
        glGetShaderiv(handle,GL_INFO_LOG_LENGTH,&succ);
        std::vector<char> log(succ);
        glGetShaderInfoLog(handle,succ,&succ,log.data());
        glDeleteShader(handle);
        cWarning("Failed to compile shader %i: %s\n",handle,log.data());
        return false;
    }
    return true;
}

bool coffee_program_link_checklog(GLuint handle){
    GLint succ = 0;
    glGetProgramiv(handle,GL_LINK_STATUS,&succ);
    if(!succ){
        glGetProgramiv(handle,GL_INFO_LOG_LENGTH,&succ);
        std::vector<char> log(succ);
        glGetProgramInfoLog(handle,succ,&succ,log.data());
        glDeleteProgram(handle);
        cWarning("Failed to link program %i: %s\n",handle,log.data());
        return false;
    }
    return true;
}

void coffee_graphics_alloc(CPipeline *pl)
{
    glCreateProgramPipelines(1,&pl->handle);
}

void coffee_graphics_alloc(CShaderProgram *shd, bool separable = true)
{
    shd->handle = glCreateProgram();
    if(separable)
        glProgramParameteri(shd->handle,GL_PROGRAM_SEPARABLE,1);
}

bool coffee_graphics_restore(
        CShaderProgram *prg, CResources::CResource *rsc)
{
    if(rsc->size<sizeof(GLenum)){
        cWarning("Shader program file not found\n");
        return false;
    }
    GLenum format = GL_NONE;
    memcpy(&format,rsc->data,sizeof(GLenum));
    const byte* bytes = (const byte*)rsc->data;
    coffee_graphics_alloc(prg);
    glProgramBinary(prg->handle,format,&bytes[sizeof(GLenum)],rsc->size-sizeof(GLenum));
    return coffee_program_link_checklog(prg->handle);
}

void coffee_graphics_store(CShaderProgram *prg, CResources::CResource *rsc)
{
    GLint psize = 0;
    glGetProgramiv(prg->handle,GL_PROGRAM_BINARY_LENGTH,&psize);
    rsc->data = malloc(psize+sizeof(GLenum));
    rsc->size = psize+sizeof(GLenum);
    GLenum format = GL_NONE;
    byte* data = (byte*)(rsc->data);
    glGetProgramBinary(prg->handle,psize,nullptr,&format,&data[sizeof(GLenum)]);
    memcpy(data,&format,sizeof(GLenum));
}

void coffee_graphics_free(CPipeline *pl)
{
    glDeleteProgramPipelines(1,&pl->handle);
    pl->handle = 0;
}

void coffee_graphics_free(CShader *shd)
{
    glDeleteShader(shd->handle);
    shd->handle = 0;
}

void coffee_graphics_free(CShaderProgram *prg)
{
    glDeleteProgram(prg->handle);
    prg->handle = 0;
}

void coffee_graphics_free(CShaderStageProgram *prg)
{
    glDeleteProgram(prg->handle);
    prg->handle = 0;
}

void coffee_graphics_bind(CPipeline *pl)
{
    glBindProgramPipeline(pl->handle);
}

void coffee_graphics_unbind(CPipeline *pl)
{
    glBindProgramPipeline(0);
}

bool coffee_graphics_shader_compile(
        CShaderStageProgram *prg, cstring rsc,
        GLenum type, UseProgramStageMask stage)
{
    prg->stage = stage;
    prg->handle = glCreateShaderProgramv(type,1,&rsc);
    return coffee_program_link_checklog(prg->handle);
}

bool coffee_graphics_shader_compile(
        CShader *prg, cstring rsc,
        GLenum stage, UseProgramStageMask stageMask)
{
    prg->stage = stageMask;
    prg->handle = glCreateShader(stage);
    glShaderSource(prg->handle,1,&rsc,nullptr);
    glCompileShader(prg->handle);
    return coffee_shader_compile_checklog(prg->handle);
}

bool coffee_graphics_shader_compile(
        CShader *prg, CResources::CResource* res,
        GLenum stage, UseProgramStageMask stageMask)
{
    if(res->size==0)
        return false;
    return coffee_graphics_shader_compile(
                prg,(cstring)res->data,
                stage,stageMask);
}

bool coffee_graphics_shader_compile(
        CShaderStageProgram *prg, CResources::CResource* rsc,
        GLenum stage, UseProgramStageMask stageMask)
{
    if(rsc->size==0)
        return false;
    return coffee_graphics_shader_compile(
                prg,(cstring)rsc->data,
                stage,stageMask);
}

void coffee_graphics_shader_link(
        CShaderProgram *prg)
{
    glLinkProgram(prg->handle);
    coffee_program_link_checklog(prg->handle);
}

void coffee_graphics_shader_attach(
        CShaderProgram *shd, CShader *stg)
{
    glAttachShader(shd->handle,stg->handle);
    shd->stages = shd->stages|stg->stage;
}

void coffee_graphics_shader_detach(
        CShaderProgram *shd, CShader *stg)
{
    glDetachShader(shd->handle,stg->handle);
    shd->stages = shd->stages&(~stg->stage);
}

void coffee_graphics_shader_attach(
        CPipeline *pl,
        CShaderStageProgram *stg, UseProgramStageMask filter)
{
    glUseProgramStages(pl->handle,stg->stage&filter,stg->handle);
}

void coffee_graphics_shader_attach(
        CPipeline *pl, CShaderProgram *stg, UseProgramStageMask filter)
{
    glUseProgramStages(pl->handle,stg->stages&filter,stg->handle);
}

void coffee_graphics_shader_uniform_block_get(
        CShaderProgram *prg, cglstring name, GLuint* loc)
{
    *loc = glGetUniformBlockIndex(prg->handle,name);
}

void coffee_graphics_shader_uniform_block_set(
        CShaderProgram *prg, const CUniformBlockBinding &block)
{
    glUniformBlockBinding(prg->handle,block.object->index,block.binding);
}

GLint coffee_graphics_shader_uniform_value_get(
        CShaderProgram *prg, cglstring name)
{
    return glGetUniformLocation(prg->handle,(const GLchar*)name);
}

UseProgramStageMask operator~(UseProgramStageMask msk)
{
    return (UseProgramStageMask)(~(uint32)msk);
}

} // namespace CGraphicsWrappers
} // namespace Coffee

