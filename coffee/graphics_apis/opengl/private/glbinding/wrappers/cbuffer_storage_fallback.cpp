#include <coffee/graphics_apis/opengl/glbinding/wrappers/cbuffer_storage_fallback.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

void *coffee_graphics_buffer_map_safe(CBuffer &buf, const CBufferAccess &mask)
{
    buf.mapflags = mask;
    coffee_graphics_bind(buf);
    glMapBufferRange(gl_get(buf.type),
                     0,buf.size,
                     gl_get(mask));
    coffee_graphics_unbind(buf);
    return buf.data;
}

bool coffee_graphics_buffer_unmap_safe(CBuffer &buf)
{
    if(!buf.data)
        return true;
    coffee_graphics_bind(buf);
    GLboolean b = glUnmapBuffer(gl_get(buf.type));
    buf.data = nullptr;
    coffee_graphics_unbind(buf);
    return b == GL_TRUE;
}

void* coffee_graphics_buffer_download_buffer_safe(
        CBuffer &buf, const CGszptr &offset, const CGsize &size)
{
    void* data = Alloc(size);
    coffee_graphics_bind(buf);
    glGetBufferSubData(
                gl_get(buf.type),
                offset,size,data);
    coffee_graphics_unbind(buf);
    return data;
}

void coffee_graphics_buffer_store_safe(
        CBuffer &buf, const void *data, const CGsize &size,
        const CBufferUsage &usage)
{
    buf.size = size;
    coffee_graphics_bind(buf);
    glBufferData(
                gl_get(buf.type),
                size,data,
                gl_get(usage));
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_store_immutable_safe(
        CBuffer &buf, const void *data, const CGsize &size,
        const CBufferStorage &usage)
{
    buf.size = size;
    coffee_graphics_bind(buf);
    glBufferStorage(
                gl_get(buf.type),
                size,data,
                gl_get(usage));
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_substore_safe(
        CBuffer &buf, const void *data,
        const CGszptr &offset, const CGsize &size)
{
    coffee_graphics_bind(buf);
    glBufferSubData(
                gl_get(buf.type),
                offset,size,data);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_buffer_copy_safe(
        CBuffer const& buf1, CBuffer& buf2,
        CGszptr const& offset1, CGszptr const& offset2,
        CGsize const& size)
{
    glBindBuffer(GL_COPY_READ_BUFFER,buf1.handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER,buf2.handle);
    glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,offset1,offset2,size);
    glBindBuffer(GL_COPY_READ_BUFFER,0);
    glBindBuffer(GL_COPY_WRITE_BUFFER,0);
}

void coffee_graphics_buffer_invalidate_safe(CBuffer &buf)
{
    coffee_graphics_buffer_store_safe(buf,nullptr,0,CBufferUsage::StaticDraw);
}

}
}
