#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

CUniformValue::CUniformValue():
    name(nullptr),
    size(0),
    data(nullptr),
    flags(0),
    location(-1)
{
}

CUniformBlock::CUniformBlock():
    blockBinding(0),
    shaderIndex(0),
    size(0),
    name(nullptr),
    buffer(nullptr)
{
}

void coffee_shader_apply_uniform(GLuint program, const CUniformValue &val)
{
    GLboolean matrix_transpose = GL_FALSE;
    if(flags&MatrixTranspose)
        matrix_transpose = GL_TRUE;
    const GLfloat* data = reinterpret_cast<const GLfloat*>(this->data);
    switch(size){
    case sizeof(GLfloat):
        glProgramUniform1fv(program,location,1,data);
        break;
    case sizeof(GLfloat)*2:
        glProgramUniform2fv(program,location,1,data);
        break;
    case sizeof(GLfloat)*3:
        glProgramUniform3fv(program,location,1,data);
        break;
    case sizeof(GLfloat)*4:
        glProgramUniform4fv(program,location,1,data);
        break;
    case sizeof(GLfloat)*9:
        glProgramUniformMatrix3fv(program,location,1,matrix_transpose,data);
        break;
    case sizeof(GLfloat)*16:
        glProgramUniformMatrix4fv(program,location,1,matrix_transpose,data);
        break;
    }
}

}
}
