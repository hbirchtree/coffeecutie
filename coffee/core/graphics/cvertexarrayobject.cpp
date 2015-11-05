#include "cvertexarrayobject.h"

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
    type(GL_NONE),
    normalized(GL_FALSE)
{
}

CVertexArrayObject::CVertexArrayObject():
    handle(0)
{
}

void coffee_graphics_vao_attribute_format(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexFormat &fmt)
{
    glVertexArrayAttribFormat(
                vao->handle,
                attr.attribIdx,fmt.size,fmt.type,
                fmt.normalized,fmt.offset);
}

void coffee_graphics_vao_attribute_buffer(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexBufferBinding &buf)
{
    glEnableVertexArrayAttrib(vao->handle,attr.attribIdx);
    glVertexArrayAttribBinding(vao->handle,attr.attribIdx,buf.binding);
    glVertexArrayBindingDivisor(vao->handle,buf.binding,buf.divisor);
}

void coffee_graphics_vao_attribute_bind_buffer(CVertexArrayObject *vao, const CVertexBufferBinding &buf)
{
    glVertexArrayVertexBuffer(vao->handle,buf.binding,buf.buffer->handle,buf.offset,buf.stride);
}

void coffee_graphics_alloc(CVertexArrayObject *vao)
{
    glGenVertexArrays(1,&vao->handle);
}

void coffee_graphics_free(CVertexArrayObject *vao)
{
    glDeleteVertexArrays(1,&vao->handle);
}

void coffee_graphics_activate(const CVertexArrayObject *vao)
{
    coffee_graphics_bind(vao);
    coffee_graphics_unbind(vao);
}

void coffee_graphics_bind(const CVertexArrayObject *vao)
{
    glBindVertexArray(vao->handle);
}

void coffee_graphics_unbind(const CVertexArrayObject*)
{
    glBindVertexArray(0);
}

}
}
