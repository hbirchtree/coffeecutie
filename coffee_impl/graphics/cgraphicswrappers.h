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

struct CGLReport
{
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    const char* message;
};

struct CGLState
        //A partial state dump of OpenGL bindings and etc.
{
    GLuint      vertex_array    = 0;
    GLuint      array_buffer    = 0;
    GLuint      uniform_buffer  = 0;
    GLuint      element_buffer  = 0;
    GLuint      program_obj     = 0;
    GLuint      pipeline_obj    = 0;
};

struct CBuffer{
    GLuint      handle      = 0;
    GLsizeiptr  size        = 0;
    GLenum      bufferType  = GL_NONE;
    BufferStorageMask flags = (BufferStorageMask)0;

    void create(){
        glGenBuffers(1,&handle);
    }
    void free(){
        glDeleteBuffers(1,&handle);
        handle = 0;
    }

    void bind(){
        glBindBuffer(bufferType,handle);
    }
    void bindRange(GLuint index, GLsizeiptr offset, GLsizeiptr size){
        glBindBufferRange(bufferType,index,handle,offset,size);
    }
    void bindRange(GLuint index){ //Convenience for typical buffers
        bindRange(index,0,size);
    }
    void unbind(){
        glBindBuffer(bufferType,0);
    }

    bool isValid(){
        return glIsBuffer(handle)==GL_TRUE;
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

    void fetch(GLenum type, GLsizeiptr offset,
               GLsizeiptr size, GLvoid* data)
    {
        glGetBufferSubData(type,offset,size,data);
    }
    void fetch(GLsizeiptr offset, GLsizeiptr size, GLvoid* data)
    {
        fetch(bufferType,offset,size,data);
    }

    void invalidate(GLintptr offset, GLsizeiptr size){
        glInvalidateBufferSubData(handle,offset,size);
    }
};

struct CSubBuffer
{
    CBuffer*    parent      = nullptr;
    void*       dataPtr     = nullptr;
    GLsizeiptr  size        = 0;
    GLenum      bufferType  = GL_NONE;
};

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

struct CUniformValue{
    enum UniformFlags{
        BlockFlag       = 0b1, //Part of a uniform block
        MatrixTranspose = 0b10,
    };

    cstring name    = nullptr;
    uint8_t size    = 0; //Uniforms should not be larger than this, right? Right..?
    void* data      = nullptr;
    uint8_t flags   = 0;
    GLint location  =-1;

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
    GLuint blockBinding         = 0;
    GLuint shaderIndex          = 0;

    uint32_t size               = 0;
    cstring name                = nullptr;
};

struct CFramebuffer; //Implement this!
}
}

#endif // CGRAPHICSWRAPPERS_H
