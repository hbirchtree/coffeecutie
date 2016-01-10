#include <glbinding/wrappers/cshader.h>

#include <glfunctions.h>
#include "coffee/core/base/cdebug.h"

namespace Coffee {
namespace CGraphicsWrappers {

bool coffee_shader_compile_checklog(GLuint handle){
    GLint succ = 0;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&succ);
    if(!succ){
        glGetShaderiv(handle,GL_INFO_LOG_LENGTH,&succ);
        std::vector<char> log(succ);
        glGetShaderInfoLog(handle,succ,&succ,log.data());
        glDeleteShader(handle);
        cWarning("Failed to compile shader {0}: {1}\n",handle,log.data());
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
        cWarning("Failed to link program {0}: {1}\n",handle,log.data());
        return false;
    }
    return true;
}

void coffee_graphics_alloc(size_t count, CPipeline *pl)
{
    CGuint *handles = new CGuint[count];
    glGenProgramPipelines(count,handles);
    for(size_t i=0;i<count;i++)
    {
        pl[i].handle = handles[i];
        coffee_graphics_bind(pl[i]);
    }
    glBindProgramPipeline(0);
    delete[] handles;
}

void coffee_graphics_alloc(size_t count, CShaderProgram *shd, bool separable)
{
    for(size_t i=0;i<count;i++)
    {
        shd[i].handle = glCreateProgram();
        if(separable)
            glProgramParameteri(shd[i].handle,GL_PROGRAM_SEPARABLE,1);
    }
}

bool coffee_graphics_restore(
        CShaderProgram &prg, const CResources::CResource &rsc)
{
    if(rsc.size<sizeof(GLenum)){
        cWarning("Shader program file not found\n");
        return false;
    }
    GLenum format = GL_NONE;
    CMemCpy(&format,rsc.data,sizeof(GLenum));
    const byte_t* bytes = (const byte_t*)rsc.data;
    coffee_graphics_alloc(prg);
    glProgramBinary(prg.handle,format,&bytes[sizeof(GLenum)],rsc.size-sizeof(GLenum));
    return coffee_program_link_checklog(prg.handle);
}

bool coffee_graphics_store(const CShaderProgram &prg, CResources::CResource &rsc)
{
    GLint psize = 0;
    glGetProgramiv(prg.handle,GL_PROGRAM_BINARY_LENGTH,&psize);

    if(psize<1)
        return false;

    rsc.data = Alloc(psize+sizeof(GLenum));
    rsc.size = psize+sizeof(GLenum);

    GLenum format = GL_NONE;
    byte_t* data = (byte_t*)(rsc.data);
    glGetProgramBinary(prg.handle,psize,nullptr,&format,&data[sizeof(GLenum)]);
    CMemCpy(data,&format,sizeof(GLenum));
    return true;
}

void coffee_graphics_free(size_t count, CPipeline *pl)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = pl[i].handle;
        pl[i].handle = 0;
    }
    glDeleteProgramPipelines(count,handles);
    delete[] handles;
}

void coffee_graphics_free(size_t count, CShader *shd)
{
    for(size_t i=0;i<count;i++)
    {
        glDeleteShader(shd[i].handle);
        shd[i].handle = 0;
    }
}

void coffee_graphics_free(size_t count, CShaderProgram *prg)
{
    for(size_t i=0;i<count;i++)
    {
        glDeleteProgram(prg[i].handle);
        prg[i].handle = 0;
    }
}

void coffee_graphics_free(size_t count, CShaderStageProgram *prg)
{
    for(size_t i=0;i<count;i++)
    {
        glDeleteProgram(prg[i].handle);
        prg[i].handle = 0;
    }
}

void coffee_graphics_bind(CPipeline &pl)
{
    glBindProgramPipeline(pl.handle);
}

void coffee_graphics_unbind(CPipeline&)
{
    glBindProgramPipeline(0);
}

bool coffee_graphics_shader_compile(
        CShaderStageProgram &prg, cstring rsc,
        const CProgramStage &type)
{
    prg.stage = type;
    prg.handle = glCreateShaderProgramv(
                gl_get(type),
                1,&rsc);
    return coffee_program_link_checklog(prg.handle);
}

bool coffee_graphics_shader_compile(
        CShader &prg, cstring rsc,
        CProgramStage const& stage)
{
    prg.stage = stage;
    prg.handle = glCreateShader(
                gl_get(stage));
    glShaderSource(prg.handle,1,&rsc,nullptr);
    glCompileShader(prg.handle);
    return coffee_shader_compile_checklog(prg.handle);
}

bool coffee_graphics_shader_compile(
        CShader &prg, const CResources::CResource& res,
        CProgramStage const& stage)
{
    if(res.size==0)
        return false;
    return coffee_graphics_shader_compile(
                prg,(cstring)res.data,
                stage);
}

bool coffee_graphics_shader_compile(
        CShaderStageProgram &prg, CResources::CResource* rsc,
        CProgramStage const& stage)
{
    if(rsc->size==0)
        return false;
    return coffee_graphics_shader_compile(
                prg,(cstring)rsc->data,
                stage);
}

void coffee_graphics_shader_link(CShaderProgram &prg)
{
    glLinkProgram(prg.handle);
    coffee_program_link_checklog(prg.handle);
}

void coffee_graphics_shader_attach(
        CShaderProgram &shd, CShader &stg)
{
    glAttachShader(shd.handle,stg.handle);
    shd.stages = shd.stages|stg.stage;
}

void coffee_graphics_shader_detach(
        CShaderProgram &shd, CShader &stg)
{
    glDetachShader(shd.handle,stg.handle);
    shd.stages = shd.stages&(~stg.stage);
}

void coffee_graphics_shader_attach(
        CPipeline &pl,
        CShaderStageProgram &stg,
        const CProgramStage &filter)
{
    glUseProgramStages(pl.handle,
                       gl_getf(stg.stage&filter),
                       stg.handle);
}

void coffee_graphics_shader_attach(
        CPipeline &pl,
        CShaderProgram &stg,
        const CProgramStage &filter)
{
    glUseProgramStages(pl.handle,
                       gl_getf(stg.stages&filter),
                       stg.handle);
}

CGhnd coffee_graphics_shader_uniform_value_get(CShaderProgram *prg, cstring name)
{
    return glGetUniformLocation(prg->handle,(const GLchar*)name);
}

} // namespace CGraphicsWrappers
} // namespace Coffee

