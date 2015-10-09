#include "cvertexarrayobject.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_vao_attribute_format(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexFormat &fmt)
{
    glVertexArrayAttribFormat(
                vao->handle,
                attr.attribIdx,fmt.size,fmt.type,
                fmt.normalized,fmt.offset);
}

void coffee_vao_attribute_buffer(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexBufferBinding &buf)
{
    glEnableVertexArrayAttrib(vao->handle,attr.attribIdx);
    glVertexArrayAttribBinding(vao->handle,attr.attribIdx,buf.binding);
    glVertexArrayBindingDivisor(vao->handle,buf.binding,buf.divisor);
}

void coffee_vao_attribute_bind_buffer(CVertexArrayObject *vao, const CVertexBufferBinding &buf)
{
    glVertexArrayVertexBuffer(vao->handle,buf.binding,buf.buffer->handle,buf.offset,buf.stride);
}

}
}
