#include <glbinding/wrappers/cbuffer_storage.h>

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

void *coffee_graphics_buffer_map(CBuffer &buf, const CBufferAccess &mask)
{
    buf.mapflags = mask;
    buf.data = glMapNamedBufferRange(
                buf.handle,0,buf.size,
                gl_get(mask));
    return buf.data;
}

bool coffee_graphics_buffer_unmap(CBuffer &buf)
{
    if(!buf.data)
        return true;
    GLboolean b = glUnmapNamedBuffer(buf.handle);
    buf.data = nullptr;
    return b == GL_TRUE;
}

void *coffee_graphics_buffer_download_buffer(
        CBuffer &buf, const CGszptr &offset, const CGsize &size)
{
    void* data = Alloc(size);
    glGetNamedBufferSubData(buf.handle,offset,size,data);
    return data;
}

void coffee_graphics_buffer_store(
        CBuffer &buf, c_cptr data, const CGsize &size,
        const CBufferUsage &usage)
{
    buf.size = size;
    glNamedBufferData(
                buf.handle,size,data,
                gl_get(usage));
}

void coffee_graphics_buffer_store_immutable(
        CBuffer &buf, c_cptr data, const CGsize &size,
        const CBufferStorage &usage)
{
    coffee_graphics_activate(buf);
    buf.size = size;
    glNamedBufferStorage(buf.handle,size,data,
                         gl_get(usage));
}

void coffee_graphics_buffer_substore(
        CBuffer &buf, c_cptr data,
        const CGszptr &offset, const CGsize &size)
{
    glNamedBufferSubData(buf.handle,offset,size,data);
}

void coffee_graphics_buffer_copy(
        CBuffer const& buf1, CBuffer &buf2,
        const CGszptr &offset1, const CGszptr &offset2,
        const CGsize &size)
{
    glCopyNamedBufferSubData(buf1.handle,buf2.handle,offset1,offset2,size);
}

void coffee_graphics_buffer_invalidate(CBuffer &buf)
{
    glInvalidateBufferData(buf.handle);
}

void coffee_graphics_buffer_bind_range(
        const _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_section> &binding)
{
    glBindBufferRange(gl_get(binding.resource->type),binding.index,
                      binding.resource->handle,binding.resource->offset,
                      binding.resource->size);
}

void coffee_graphics_buffer_bind_range(
        const CBuffer &buffer, const CBufferType &type,
        const CGszptr &offset, const CGsize &size, const CGuint &index)
{
    glBindBufferRange(gl_get(type),index,buffer.handle,
                      offset,size);
}

}
}
