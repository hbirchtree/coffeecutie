#include "cbuffer.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

CBuffer::CBuffer()
{
    this->data = nullptr;
    this->handle = 0;
    this->size = 0;
    this->type = CBufferType::None;
}

CSubBuffer::CSubBuffer()
{
    this->handle = 0;
    this->offset = 0;
    this->parent = nullptr;
    this->size = 0;
    this->type = CBufferType::None;
}

void coffee_graphics_buffer_bind_range(CBuffer *buf, GLuint index, GLenum bufferType, CGszptr offset, CGsize size)
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
    glBindBuffer(coffee_get(buf->type,cbuffertype_map),buf->handle);
}

void coffee_graphics_unbind(CBuffer *buf)
{
    glBindBuffer(coffee_get(buf->type,cbuffertype_map),0);
}

void *coffee_graphics_buffer_map(CBuffer *buf, CBufferAccess mask)
{
    buf->mapflags = mask;
    buf->data = glMapNamedBufferRange(
                buf->handle,0,buf->size,
                coffee_get_flags(mask,cbufferaccess_map));
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

void *coffee_graphics_buffer_map_safe(CBuffer *buf, CBufferAccess mask)
{
    buf->mapflags = mask;
    coffee_graphics_bind(buf);
    glMapBufferRange(coffee_get(buf->type,cbuffertype_map),
                     0,buf->size,
                     coffee_get_flags(mask,cbufferaccess_map));
    coffee_graphics_unbind(buf);
    return buf->data;
}

bool coffee_graphics_buffer_unmap_safe(CBuffer *buf)
{
    if(!buf->data)
        return true;
    coffee_graphics_bind(buf);
    GLboolean b = glUnmapBuffer(coffee_get(buf->type,cbuffertype_map));
    buf->data = nullptr;
    coffee_graphics_unbind(buf);
    return b == GL_TRUE;
}

void *coffee_graphics_buffer_download_buffer(CBuffer *buf, CGszptr offset, CGsize size)
{
    void* data = malloc(size);
    glGetNamedBufferSubData(buf->handle,offset,size,data);
    return data;
}

void* coffee_graphics_buffer_download_buffer_safe(CBuffer *buf, CGszptr offset, CGsize size)
{
    void* data = malloc(size);
    coffee_graphics_bind(buf);
    glGetBufferSubData(
                coffee_get(buf->type,cbuffertype_map),
                offset,size,data);
    coffee_graphics_unbind(buf);
    return data;
}

void coffee_graphics_buffer_store(
        CBuffer *buf,
        const void *data, CGsize size,
        CBufferUsage usage = CBufferUsage::Default)
{
    buf->size = size;
    glNamedBufferData(buf->handle,size,data,usage);
}

void coffee_graphics_buffer_store_safe(CBuffer *buf,
        const void *data, CGsize size,
        CBufferUsage usage = CBufferUsage::Default)
{
    buf->size = size;
    coffee_graphics_bind(buf);
    glBufferData(
                buf->type,size,data,
                usage);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_substore(
        CBuffer *buf, const void *data, CGszptr offset, CGsize size)
{
    glNamedBufferSubData(buf->handle,offset,size,data);
}

void coffee_graphics_buffer_substore_safe(CBuffer *buf, const void *data,
        CGszptr offset, CGsize size)
{
    coffee_graphics_bind(buf);
    glBufferSubData(buf->type,offset,size,data);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_copy(
        GLuint buf1, GLuint buf2, CGszptr offset1,
        CGszptr offset2, CGsize size)
{
    glCopyNamedBufferSubData(buf1,buf2,offset1,offset2,size);
}

void coffee_graphics_buffer_copy_safe(
        GLuint buf1, GLuint buf2,
        CGszptr offset1, CGszptr offset2, CGsize size)
{
    glBindBuffer(GL_COPY_READ_BUFFER,buf1);
    glBindBuffer(GL_COPY_WRITE_BUFFER,buf2);
    glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,offset1,offset2,size);
    glBindBuffer(GL_COPY_READ_BUFFER,0);
    glBindBuffer(GL_COPY_WRITE_BUFFER,0);
}

void coffee_graphics_buffer_copy(
        CBuffer *buf1, CBuffer *buf2,
        CGszptr offset1, CGszptr offset2, CGsize size)
{
    coffee_graphics_buffer_copy(buf1->handle,buf2->handle,offset1,offset2,size);
}

void coffee_graphics_buffer_copy_safe(
        CBuffer *buf1, CBuffer *buf2,
        CGszptr offset1, CGszptr offset2, CGsize size)
{
    coffee_graphics_buffer_copy_safe(buf1->handle,buf2->handle,offset1,offset2,size);
}

void coffee_graphics_buffer_resize(
        CBuffer *buf, CGszptr oldOffset,
        CGsize oldSize, CGszptr targetOffset, CGsize newSize)
{
    GLuint old = buf->handle;

    coffee_graphics_alloc(buf);
    coffee_graphics_buffer_store(buf,nullptr,newSize,CBufferUsage::DynamicDraw);
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

void coffee_graphics_buffer_store_immutable(CBuffer *buf, const void *data, CGsize size,
        CBufferStorage usage = CBufferStorage::WriteBit)
{
    coffee_graphics_activate(buf);
    buf->size = size;
    glNamedBufferStorage(buf->handle,size,data,usage);
}

void coffee_graphics_buffer_store_immutable_safe(
        CBuffer *buf, const void *data, CGsize size,
        BufferStorageMask usage = GL_NONE_BIT)
{
    buf->size = size;
    coffee_graphics_bind(buf);
    glBufferStorage(buf->type,size,data,usage);
    coffee_graphics_unbind(buf);
}

void *coffee_graphics_buffer_sub_data(CSubBuffer *buf)
{
    if(!buf->parent->data)
        return nullptr;
    return &((byte*)(buf->parent->data))[buf->offset];
}

void coffee_graphics_buffer_sub_bind(
        const _cbasic_graphics_buffer_section *buf,
        const _cbasic_graphics_buffer_resource_desc* binding)
{
    glBindBufferRange(buf->type,binding->index,buf->parent->handle,buf->offset,buf->size);
}

void coffee_graphics_buffer_sub_unbind(
        const _cbasic_graphics_buffer_section *buf,
        const _cbasic_graphics_buffer_resource_desc* binding)
{
    glBindBufferRange(buf->type,binding->index,0,0,0);
}

void coffee_graphics_free(int count, CBuffer *buf)
{
    GLuint *handles = new GLuint[count];
    for(int i=0;i<count;i++)
    {
        handles[i] = buf[i].handle;
        buf[i].handle = 0;
    }
    glDeleteBuffers(count,handles);
    delete[] handles;
}

void coffee_graphics_alloc(int count, GLenum type, CBuffer *buf)
{
    GLuint handles[count];
    glGenBuffers(count,handles);
    for(int i=0;i<count;i++)
    {
        buf[i].handle = handles[i];
        buf[i].type = type;
    }
}

}
}
