#include "cshader.h"

namespace Coffee {
namespace CGraphicsWrappers {

bool CPipeline::createPipeline()
{
    glGenProgramPipelines(1,&handle);
    return handle != 0;
}

void CPipeline::attachShader(CShader *shader)
{
    glUseProgramStages(handle,shader->stage,shader->handle);
}

bool CShader::compile(CResources::CResource *res, GLenum stage, UseProgramStageMask stageMask)
{
    res->read_data(true);
    if(res->size==0||!res->data)
        return false;
    const char* str = reinterpret_cast<const char*>(res->data);
    handle = glCreateShaderProgramv(stage,1,&str);
//    glCompileShader(handle);

    GLint succ = 0;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&succ);
    if(!succ){
        glGetShaderiv(handle,GL_INFO_LOG_LENGTH,&succ);
        std::vector<char> log(succ);
        glGetShaderInfoLog(handle,succ,&succ,log.data());
        glDeleteShader(handle);
        cDebug("%s\n",log.data());
        cMsg("GL","You suck...");

        return false;
    }else{
        cMsg("GL","Success!");
    }

    this->stage = stageMask;

    return true;
}

} // namespace CGraphicsWrappers
} // namespace Coffee

