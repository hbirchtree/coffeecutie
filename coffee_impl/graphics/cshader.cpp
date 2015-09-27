#include "cshader.h"

namespace Coffee {
namespace CGraphicsWrappers {

static bool coffee_shader_compile_checklog(GLuint handle);
static bool coffee_program_link_checklog(GLuint handle);

void CPipeline::bind()
{
    glBindProgramPipeline(handle);
}

void CPipeline::unbind()
{
    glBindProgramPipeline(0);
}

bool CPipeline::create()
{
    glGenProgramPipelines(1,&handle);
    return handle != 0;
}

void CPipeline::free()
{
    glDeleteProgramPipelines(1,&handle);
    handle = 0;
}

void CPipeline::attachStages(CShaderStageProgram *shader, UseProgramStageMask bits)
{
    cDebug("Pipeline attachment(SSP): ssp=%i,ppl=%i,stg=%#x",shader->handle,handle,(uint32)bits);
    glUseProgramStages(handle,shader->stage&bits,shader->handle);
}

void CPipeline::attachProgram(CShaderProgram *shader, UseProgramStageMask bits)
{
    cDebug("Pipeline attachment: prg=%i,ppl=%i,stg=%#x",shader->handle,handle,(uint32)bits);
    glUseProgramStages(handle,shader->stages&bits,shader->handle);
}

bool CShaderStageProgram::compile(CResources::CResource *res, GLenum stage, UseProgramStageMask stageMask)
{
    this->stage = stageMask;
    res->read_data(true);
    if(res->size==0||!res->data)
        return false;
    const char* str = reinterpret_cast<const char*>(res->data);
    cDebug("Creating shader: %s",glbinding::Meta::getString(stage).c_str());
    handle = glCreateShaderProgramv(stage,1,&str);
    return coffee_program_link_checklog(handle);
}

void CShaderStageProgram::free()
{
    glDeleteProgram(handle);
}

bool CShader::compile(CResources::CResource *res, GLenum stage)
{
    this->stage = stage;
    if(res->size==0||!res->data)
        return false;
    const char* str = reinterpret_cast<const char*>(res->data);
    cMsg("GL","Creating shader: %s",glbinding::Meta::getString(stage).c_str());

    handle = glCreateShader(stage);
    glShaderSource(handle,1,&str,nullptr);
    glCompileShader(handle);

    return coffee_shader_compile_checklog(handle);
}

void CShader::free()
{
    glDeleteShader(handle);
    handle = 0;
}

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

void CShaderProgram::create(bool separable)
{
    handle = glCreateProgram();
    if(separable)
        glProgramParameteri(handle,GL_PROGRAM_SEPARABLE,1);
}

void CShaderProgram::attachShader(CShader *shader, UseProgramStageMask maskopt)
{
    glAttachShader(handle,shader->handle);
    stages = stages|maskopt;
}

void CShaderProgram::detachShader(CShader *shader)
{
    glDetachShader(handle,shader->handle);
}

void CShaderProgram::link()
{
    glLinkProgram(handle);
    coffee_program_link_checklog(handle);
}

void CShaderProgram::free()
{
    glDeleteProgram(handle);
}

void CShaderProgram::storeProgram(CResources::CResource *out)
{
    GLint psize = 0;
    glGetProgramiv(handle,GL_PROGRAM_BINARY_LENGTH,&psize);
    out->free_data();
    out->data = malloc(psize+sizeof(GLenum));
    out->size = psize+sizeof(GLenum);
    GLenum format = GL_NONE;
    char* data = reinterpret_cast<char*>(out->data);
    glGetProgramBinary(handle,psize,nullptr,&format,&data[sizeof(GLenum)]);
    memcpy(data,&format,sizeof(GLenum));
}

bool CShaderProgram::fetchProgram(CResources::CResource *in)
{
    if(in->size<sizeof(GLenum)){
        cWarning("Shader program file not found\n");
        return false;
    }
    GLenum format = GL_NONE;
    memcpy(&format,in->data,sizeof(GLenum));
    const char* bytes = reinterpret_cast<const char*>(in->data);
    create();
    glProgramBinary(handle,format,&bytes[sizeof(GLenum)],in->size-sizeof(GLenum));
    return coffee_program_link_checklog(handle);
}

} // namespace CGraphicsWrappers
} // namespace Coffee

