#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

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
    uint32_t    size    = 0;
    void*       dataPtr = nullptr;

    GLenum      bufferType = GL_ARRAY_BUFFER;
};

struct CVertexArrayObject{
    GLuint      handle  = 0;

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
                      GLuint stride, GLvoid* pointer){
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,size,type,normalized,stride,pointer);
    }
    void addAttributeDivided(GLuint index, GLenum type,
                             GLboolean normalized, GLuint size,
                             GLuint stride, GLuint divisor,
                             GLvoid* pointer){
        addAttribute(index,type,normalized,size,stride,pointer);
        glVertexAttribDivisor(index,divisor);
    }
};

struct CUniformBlock{

};

struct CUniformValue{

};

struct CShader;
struct CFramebuffer;
}
}

#endif // CGRAPHICSWRAPPERS_H
