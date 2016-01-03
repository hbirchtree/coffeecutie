#include <glbinding/wrappers/cbuffer.h>

#include "glfunctions.h"
#include <coffee/core/base/cdebug.h>

namespace Coffee{
namespace CGraphicsWrappers{

CBuffer::CBuffer()
{
    this->data = nullptr;
    this->handle = 0;
    this->size = 0;
    this->type = CBufferType::None;
}

void coffee_graphics_alloc(size_t count, CBuffer *buf, CBufferType const& type)
{
    CGuint *handles = new CGuint[count];
    glGenBuffers(count,handles);
    for(size_t i=0;i<count;i++)
    {
        buf[i].handle = handles[i];
        buf[i].type = type;

        coffee_graphics_activate(buf[i]);
    }
    delete[] handles;
}

void coffee_graphics_free(size_t count, CBuffer *buf)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = buf[i].handle;
        buf[i].handle = 0;
    }
    glDeleteBuffers(count,handles);
    delete[] handles;
}

void coffee_graphics_activate(CBuffer &buf)
{
    coffee_graphics_bind(buf);
    coffee_graphics_unbind(buf);
}

void coffee_graphics_bind(CBuffer &buf)
{
    glBindBuffer(gl_get(buf.type),
                 buf.handle);
}

void coffee_graphics_unbind(CBuffer &buf)
{
    glBindBuffer(gl_get(buf.type),
                 0);
}

}
}
