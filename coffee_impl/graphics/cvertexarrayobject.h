#ifndef CVERTEXARRAYOBJECT
#define CVERTEXARRAYOBJECT

#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

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
                      GLsizeiptr stride, GLsizeiptr pointer)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,size,type,normalized,stride,(GLvoid*)pointer);
    }
    void addAttributeDivided(GLuint index, GLenum type,
                             GLboolean normalized, GLuint size,
                             GLsizeiptr stride, GLuint divisor,
                             GLsizeiptr pointer)
    {
        addAttribute(index,type,normalized,size,stride,pointer);
        glVertexAttribDivisor(index,divisor);
    }
};

struct CVertexFormat
{
    GLuint      size            = 0;
    GLuint      offset          = 0;
    GLenum      type            = GL_NONE;
    GLboolean   normalized      = GL_FALSE;
};

struct CVertexBufferBinding
{
    CBuffer* buffer  = nullptr;
    GLsizei  offset  = 0;
    GLsizei  stride  = 0;
    GLuint   divisor = 0;
    GLuint   binding = 0;

    void bindBuffer(CVertexArrayObject* vao){
        glVertexArrayVertexBuffer(vao->handle,binding,buffer->handle,offset,stride);
    }
};

struct CVertexAttribute
{
    GLuint                  attribIdx   = 0;
    CVertexFormat*          fmt         = nullptr;
    CVertexBufferBinding*   bnd         = nullptr;

    void setFormat(CVertexArrayObject* vao, const CVertexFormat& fmt){
        glVertexArrayAttribFormat(vao->handle,
                                  attribIdx,fmt.size,fmt.type,fmt.normalized,fmt.offset);
    }
    void setBuffer(CVertexArrayObject* vao, const CVertexBufferBinding& buf){
        glEnableVertexArrayAttrib(vao->handle,attribIdx);
        glVertexArrayAttribBinding(vao->handle,attribIdx,buf.binding);
        glVertexArrayBindingDivisor(vao->handle,buf.binding,buf.divisor);
    }
};

}
}

#endif // CVERTEXARRAYOBJECT

