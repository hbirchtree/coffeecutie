#ifndef CVERTEXARRAYOBJECT
#define CVERTEXARRAYOBJECT

#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CVertexAttribute
{
    GLenum type         = GL_NONE;
    GLboolean normalized= GL_FALSE;

    GLuint location = 0;
    GLuint size     = 0;
    GLuint offset   = 0;
    GLuint stride   = 0;
    GLuint divisor  = 0;
};

struct CVertexArrayObject{
    GLuint      handle  = 0;

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
    void addAttribute(CVertexAttribute* attr){
        addAttributeDivided(attr->location,attr->type,attr->normalized,attr->size,
                            attr->stride,attr->divisor,(GLvoid*)attr->offset);
    }
};

}
}

#endif // CVERTEXARRAYOBJECT

