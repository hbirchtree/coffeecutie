#ifndef CUNIFORMTYPES
#define CUNIFORMTYPES

#include "coffee.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CUniformValue{
    enum UniformFlags{
        BlockFlag       = 0b1, //Part of a uniform block
        MatrixTranspose = 0b10,
    };

    cstring name    = nullptr;
    uint8   size    = 0; //Uniforms should not be larger than this, right? Right..?
    void*   data      = nullptr;
    uint8   flags   = 0;
    GLint   location  =-1;

    void applyUniform(GLuint program){
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
};

struct CUniformBlock{
    //A CBuffer is created to correspond with these objects
    //It is beneficial to create a larger buffer
    GLuint  blockBinding         = 0;
    GLuint  shaderIndex          = 0;

    uint32  size               = 0;
    cstring name                = nullptr;
};

}
}

#endif // CUNIFORMTYPES

