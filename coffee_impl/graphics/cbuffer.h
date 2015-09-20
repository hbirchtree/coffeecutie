#ifndef CBUFFER
#define CBUFFER

#include "coffee.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CBuffer{
    GLuint      handle      = 0;
    GLsizeiptr  size        = 0;
    GLenum      bufferType  = GL_NONE;
    BufferStorageMask flags = GL_NONE_BIT;

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

    void subCopy(GLuint from, GLsizeiptr roffset, GLsizeiptr woffset, GLsizeiptr size){
        glBindBuffer(GL_COPY_READ_BUFFER,from);
        glBindBuffer(GL_COPY_WRITE_BUFFER,handle);
        glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,roffset,woffset,size);
        glBindBuffer(GL_COPY_READ_BUFFER,0);
        glBindBuffer(GL_COPY_WRITE_BUFFER,0);
    }

    void resize(GLsizeiptr newsize){
        GLuint old = handle;
        GLsizeiptr osize = size;
        create();
        bind();
        store(newsize,nullptr);
        unbind();
        if(osize<newsize)
            subCopy(old,0,0,osize);
        else
            subCopy(old,0,0,newsize);
        glDeleteBuffers(1,&old);
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
    GLsizeiptr  offset      = 0;
    GLsizeiptr  size        = 0;
    GLenum      bufferType  = GL_NONE;
    GLuint      index       = 0;

    void bindParent(){
        parent->bind();
    }
    void bindRange(){ //TF and uniform buffers
        glBindBufferRange(bufferType,index,parent->handle,offset,size);
    }
    void unbind()
    {
        glBindBufferRange(bufferType,index,0,0,0);
    }

    void store(const void* data)
    {
        parent->subStore(offset,size,data);
    }
    void subStore(GLsizeiptr offset, GLsizeiptr size, const void* data)
    {
        parent->subStore(this->offset+offset,size,data);
    }
};

}
}

#endif // CBUFFER

