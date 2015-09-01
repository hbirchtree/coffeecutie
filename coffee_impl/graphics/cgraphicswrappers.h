#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "coffee.h"

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

struct CBuffer{
    GLuint      handle  = 0;
    GLsizeiptr  size    = 0;
    GLenum      bufferType = GL_ARRAY_BUFFER;
    BufferStorageMask flags = GL_DYNAMIC_STORAGE_BIT;

    void bind(){
        glBindBuffer(bufferType,handle);
    }
    void unbind(){
        glBindBuffer(bufferType,0);
    }

    void store(GLenum type, GLsizeiptr size,
               const GLvoid* data, BufferStorageMask flags)
    {
        this->bufferType = type;
        this->size = size;
        this->flags = flags;

        glBufferStorage(type,size,data,flags);
    }
    void store(GLsizeiptr size, const GLvoid* data){
        store(bufferType,size,data,flags);
    }
    void subStore(GLintptr offset, GLsizeiptr size, const GLvoid* data){
        glBufferSubData(bufferType,offset,size,data);
    }
};

struct CVertexArrayObject{
    GLuint      handle  = 0;
    CBuffer**   buffers = nullptr;

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

struct CUniformValue{
    enum UniformFlags{
        BlockFlag       = 0b1, //Part of a uniform block
    };

    CString name;
    uint8_t size    = 0; //Uniforms should not be larger than this, right? Right..?
    void* data      = nullptr;
    uint8_t flags   = 0;
};

struct CUniformBlock{
    uint32_t size   = 0;
    CString name;
    void* data      = nullptr;
    uint16_t numValues = 0;
    CUniformValue** values = nullptr;
};

struct CShader;
struct CFramebuffer;
}
}

#endif // CGRAPHICSWRAPPERS_H
