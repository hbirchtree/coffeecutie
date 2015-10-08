#ifndef CVERTEXARRAYOBJECT
#define CVERTEXARRAYOBJECT

#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CVertexAttributeData
{
};

struct CVertexArrayObject{
    GLuint      handle  = 0;

    void create(){
        glGenVertexArrays(1,&handle);
    }
    void activate(){
        bind();
        unbind();
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
};

struct CVertexAttribute
{
    GLuint                  attribIdx   = 0;
    CVertexFormat*          fmt         = nullptr;
    CVertexBufferBinding*   bnd         = nullptr;
};

extern void coffee_vao_attribute_format(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexFormat &fmt);
extern void coffee_vao_attribute_buffer(
        CVertexArrayObject* vao, const CVertexAttribute &attr,
        const CVertexBufferBinding& buf);
extern void coffee_vao_attribute_bind_buffer(
        CVertexArrayObject* vao,
        const CVertexBufferBinding& buf);

}
}

#endif // CVERTEXARRAYOBJECT

