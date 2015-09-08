#ifndef CVERTEXARRAYOBJECT
#define CVERTEXARRAYOBJECT

#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CVertexArrayObject{
    GLuint      handle  = 0;
    CBuffer**   buffers = nullptr;

    void create(){
        glGenVertexArrays(1,&handle);
    }
    void free(){
        glDeleteVertexArrays(1,&handle);
    }

    void bind(){
        glBindVertexArray(handle);
    }
    void unbind(){
        glBindVertexArray(0);
    }

    bool isBound(){
        GLint bound = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&bound);
        return (GLuint)bound==handle;
    }
    bool isValid(){
        return glIsVertexArray(handle)==GL_TRUE;
    }

    void addAttribute(GLuint index, GLenum type,
                      GLboolean normalized, GLuint size,
                      GLuint stride, GLvoid* pointer)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,size,type,normalized,stride,pointer);
    }
    void addAttributeDivided(GLuint index, GLenum type,
                             GLboolean normalized, GLuint size,
                             GLuint stride, GLuint divisor,
                             GLvoid* pointer)
    {
        addAttribute(index,type,normalized,size,stride,pointer);
        glVertexAttribDivisor(index,divisor);
    }
};

}
}

#endif // CVERTEXARRAYOBJECT

