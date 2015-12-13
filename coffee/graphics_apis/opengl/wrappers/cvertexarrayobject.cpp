#include "cvertexarrayobject.h"
#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

CVertexAttribute::CVertexAttribute():
    attribIdx(0),
    fmt(nullptr),
    bnd(nullptr)
{
}

CVertexBufferBinding::CVertexBufferBinding():
    buffer(nullptr),
    offset(0),
    stride(0),
    divisor(0),
    binding(0)
{
}

CVertexFormat::CVertexFormat():
    size(0),
    offset(0),
    type(CDataType::None),
    normalized(false)
{
}

CVertexArrayObject::CVertexArrayObject():
    handle(0)
{
}

void coffee_graphics_vao_attribute_format(
        CVertexArrayObject &vao, const CVertexAttribute& attr,
        const CVertexFormat &fmt)
{
    glVertexArrayAttribFormat(
                vao.handle,
                attr.attribIdx,
                fmt.size,
                gl_get(fmt.type),
                (fmt.normalized) ? GL_TRUE : GL_FALSE,
                fmt.offset);
}

void coffee_graphics_vao_attribute_buffer(CVertexArrayObject &vao, const CVertexAttribute& attr,
        const CVertexBufferBinding &buf)
{
    glEnableVertexArrayAttrib(vao.handle,attr.attribIdx);
    glVertexArrayAttribBinding(vao.handle,attr.attribIdx,buf.binding);
    glVertexArrayBindingDivisor(vao.handle,buf.binding,buf.divisor);
}

void coffee_graphics_vao_attribute_bind_buffer(
        const CVertexArrayObject &vao,
        const CVertexBufferBinding &buf)
{
    glVertexArrayVertexBuffer(vao.handle,buf.binding,buf.buffer->handle,buf.offset,buf.stride);
}

void coffee_graphics_vao_attribute_bind_buffer(const CVertexArrayObject &vao,
        const CVertexBufferBinding &buf,
        const CBuffer &buffer)
{
    glVertexArrayVertexBuffer(vao.handle,buf.binding,buffer.handle,buf.offset,buf.stride);
}

void coffee_graphics_alloc(size_t count, CVertexArrayObject *vao)
{
    CGuint *handles = new CGuint[count];
    glGenVertexArrays(count,handles);
    for(size_t i=0;i<count;i++)
    {
        vao[i].handle = handles[i];
    }
    delete[] handles;
}

void coffee_graphics_free(size_t count, CVertexArrayObject *vao)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = vao[i].handle;
        vao[i].handle = 0;
    }
    glDeleteVertexArrays(count,handles);
    delete[] handles;
}

void coffee_graphics_activate(const CVertexArrayObject &vao)
{
    coffee_graphics_bind(vao);
    coffee_graphics_unbind(vao);
}

void coffee_graphics_bind(const CVertexArrayObject &vao)
{
    glBindVertexArray(vao.handle);
}

void coffee_graphics_unbind(const CVertexArrayObject&)
{
    glBindVertexArray(0);
}

void coffee_graphics_vao_attribute_index_buffer(
        CVertexArrayObject &vao, const CBuffer &ibuffer)
{
    glVertexArrayElementBuffer(vao.handle,ibuffer.handle);
}

}
}
