#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

CBuffer::CBuffer():
    handle(0),
    size(0),
    bufferType(GL_NONE),
    flags(GL_NONE_BIT),
    mapflags(GL_NONE_BIT),
    data(nullptr)
{
}

CSubBuffer::CSubBuffer():
    parent(nullptr),
    offset(0),
    size(0),
    bufferType(GL_NONE),
    index(0)
{
}

void coffee_graphics_buffer_bind_range(CBuffer *buf, GLuint index, GLenum bufferType, GLsizeiptr offset, GLsizeiptr size)
{
    glBindBufferRange(bufferType,index,buf->handle,offset,size);
}

void coffee_graphics_alloc(CBuffer *buf)
{
    glGenBuffers(1,&buf->handle);
}

void coffee_graphics_free(CBuffer *buf)
{
    glDeleteBuffers(1,&buf->handle);
}

void coffee_graphics_activate(CBuffer *buf)
{
    coffee_graphics_bind(buf);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_bind(CBuffer *buf)
{
    glBindBuffer(buf->bufferType,buf->handle);
}

void coffee_graphics_unbind(CBuffer *buf)
{
    glBindBuffer(buf->bufferType,0);
}

void *coffee_graphics_buffer_map(CBuffer *buf, BufferAccessMask mask)
{
    buf->mapflags = mask;
    buf->data = glMapNamedBufferRange(buf->handle,0,buf->size,mask);
    return buf->data;
}

bool coffee_graphics_buffer_unmap(CBuffer *buf)
{
    if(!buf->data)
        return true;
    GLboolean b = glUnmapNamedBuffer(buf->handle);
    buf->data = nullptr;
    return b == GL_TRUE;
}

void *coffee_graphics_buffer_map_safe(CBuffer *buf, BufferAccessMask mask)
{
    buf->mapflags = mask;
    coffee_graphics_bind(buf);
    glMapBufferRange(buf->bufferType,0,buf->size,mask);
    coffee_graphics_unbind(buf);
    return buf->data;
}

bool coffee_graphics_buffer_unmap_safe(CBuffer *buf)
{
    if(!buf->data)
        return true;
    coffee_graphics_bind(buf);
    GLboolean b = glUnmapBuffer(buf->bufferType);
    buf->data = nullptr;
    coffee_graphics_unbind(buf);
    return b == GL_TRUE;
}

void *coffee_graphics_buffer_download_buffer(
        CBuffer *buf, GLsizeiptr offset, GLsizeiptr size)
{
    void* data = malloc(size);
    glGetNamedBufferSubData(buf->handle,offset,size,data);
    return data;
}

void* coffee_graphics_buffer_download_buffer_safe(
        CBuffer *buf, GLsizeiptr offset, GLsizeiptr size)
{
    void* data = malloc(size);
    coffee_graphics_bind(buf);
    glGetBufferSubData(buf->bufferType,offset,size,data);
    coffee_graphics_unbind(buf);
    return data;
}

void coffee_graphics_buffer_store(
        CBuffer *buf,
        const void *data, GLsizeiptr size,
        GLenum usage = GL_STATIC_DRAW)
{
    buf->size = size;
    glNamedBufferData(buf->handle,size,data,usage);
}

void coffee_graphics_buffer_store_safe(
        CBuffer *buf,
        const void *data, GLsizeiptr size,
        GLenum usage = GL_STATIC_DRAW)
{
    buf->size = size;
    coffee_graphics_bind(buf);
    glBufferData(buf->bufferType,size,data,usage);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_substore(CBuffer *buf, const void *data, GLsizeiptr offset, GLsizeiptr size)
{
    glNamedBufferSubData(buf->handle,offset,size,data);
}

void coffee_graphics_buffer_substore_safe(CBuffer *buf, const void *data, GLsizeiptr offset, GLsizeiptr size)
{
    coffee_graphics_bind(buf);
    glBufferSubData(buf->bufferType,offset,size,data);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_copy(GLuint buf1, GLuint buf2, GLsizeiptr offset1, GLsizeiptr offset2, GLsizeiptr size)
{
    glCopyNamedBufferSubData(buf1,buf2,offset1,offset2,size);
}

void coffee_graphics_buffer_copy_safe(GLuint buf1, GLuint buf2, GLsizeiptr offset1, GLsizeiptr offset2, GLsizeiptr size)
{
    glBindBuffer(GL_COPY_READ_BUFFER,buf1);
    glBindBuffer(GL_COPY_WRITE_BUFFER,buf2);
    glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,offset1,offset2,size);
    glBindBuffer(GL_COPY_READ_BUFFER,0);
    glBindBuffer(GL_COPY_WRITE_BUFFER,0);
}

void coffee_graphics_buffer_copy(CBuffer *buf1, CBuffer *buf2, GLsizeiptr offset1, GLsizeiptr offset2, GLsizeiptr size)
{
    coffee_graphics_buffer_copy(buf1->handle,buf2->handle,offset1,offset2,size);
}

void coffee_graphics_buffer_copy_safe(CBuffer *buf1, CBuffer *buf2, GLsizeiptr offset1, GLsizeiptr offset2, GLsizeiptr size)
{
    coffee_graphics_buffer_copy_safe(buf1->handle,buf2->handle,offset1,offset2,size);
}

void coffee_graphics_buffer_resize(CBuffer *buf, GLsizeiptr oldOffset, GLsizeiptr oldSize, GLsizeiptr targetOffset, GLsizeiptr newSize)
{
    GLuint old = buf->handle;

    coffee_graphics_alloc(buf);
    coffee_graphics_buffer_store(buf,nullptr,newSize,GL_DYNAMIC_DRAW);
    coffee_graphics_buffer_copy(old,buf->handle,oldOffset,targetOffset,oldSize);

    glDeleteBuffers(1,&old);
}

void coffee_graphics_buffer_invalidate(CBuffer *buf)
{
    glInvalidateBufferData(buf->handle);
}

void coffee_graphics_buffer_invalidate_safe(CBuffer *buf)
{
    coffee_graphics_buffer_store_safe(buf,nullptr,0,GL_STATIC_DRAW);
}

void coffee_graphics_buffer_store_immutable(
        CBuffer *buf, const void *data, GLsizeiptr size,
        BufferStorageMask usage = GL_NONE_BIT)
{
    coffee_graphics_activate(buf);
    buf->size = size;
    glNamedBufferStorage(buf->handle,size,data,usage);
}

void coffee_graphics_buffer_store_immutable_safe(
        CBuffer *buf, const void *data, GLsizeiptr size,
        BufferStorageMask usage = GL_NONE_BIT)
{
    buf->size = size;
    coffee_graphics_bind(buf);
    glBufferStorage(buf->bufferType,size,data,usage);
    coffee_graphics_unbind(buf);
}

void *coffee_graphics_buffer_sub_data(CSubBuffer *buf)
{
    if(!buf->parent->data)
        return nullptr;
    return &((byte*)(buf->parent->data))[buf->offset];
}

void coffee_graphics_buffer_sub_bind(CSubBuffer *buf)
{
    glBindBufferRange(buf->bufferType,buf->index,buf->parent->handle,buf->offset,buf->size);
}

void coffee_graphics_buffer_sub_unbind(CSubBuffer *buf)
{
    glBindBufferRange(buf->bufferType,buf->index,0,0,0);
}

}
}
