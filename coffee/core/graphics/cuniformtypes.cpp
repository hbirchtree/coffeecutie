#include "cuniformtypes.h"

void Coffee::CGraphicsWrappers::coffee_shader_apply_uniform(gl::GLuint program, const Coffee::CGraphicsWrappers::CUniformValue &val)
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
