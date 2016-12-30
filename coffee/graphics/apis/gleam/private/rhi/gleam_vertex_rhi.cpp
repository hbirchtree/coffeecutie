#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_VertDescriptor::alloc()
{
    CGL33::VAOAlloc(1,&m_handle);
}

void GLEAM_VertDescriptor::dealloc()
{
    CGL33::VAOFree(1,&m_handle);
}

void GLEAM_VertDescriptor::addAttribute(const GLEAM_VertAttribute &attr)
{
    m_attributes.push_back(attr);
    CGL33::VAOBind(m_handle);
    CGL33::VAOEnableAttrib(attr.index());
    if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        CGL43::VAOAttribFormat(attr.index(),attr.size(),attr.type(),false,attr.offset());
    }
}

void GLEAM_VertDescriptor::bindBuffer(uint32 binding, GLEAM_ArrayBuffer &buf)
{
    CGL43::VAOBind(m_handle);
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        buf.bind();
        for(GLEAM_VertAttribute const& attr : m_attributes)
            if(binding == attr.bufferAssociation())
            {
                CGL33::VAOAttribPointer(
                            attr.index(),attr.size(),attr.type(),
                            false,attr.stride(),
                            attr.bufferOffset()+attr.offset());
                if(attr.instanced())
                    CGL33::VAODivisor(attr.index(),1);
            }
    }
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        for(GLEAM_VertAttribute const& attr : m_attributes)
            if(binding == attr.bufferAssociation())
            {
                CGL43::VAOBindVertexBuffer(binding,buf.m_handle,
                                           attr.bufferOffset(),
                                           attr.stride());
                CGL43::VAOAttribBinding(attr.index(),binding);
                if(attr.instanced())
                    CGL43::VAOBindingDivisor(attr.index(),1);
            }
    }
}

void GLEAM_VertDescriptor::setIndexBuffer(const GLEAM_ElementBuffer *buffer)
{
    m_ibuffer = buffer;
}

void GLEAM_VertDescriptor::bind()
{
    CGL33::VAOBind(m_handle);
    if(GL_CURR_API==GL_3_3||GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        if(m_ibuffer)
            m_ibuffer->bind();
    }
}

void GLEAM_VertDescriptor::unbind()
{
    CGL33::VAOBind(0);
    if(GL_CURR_API==GL_3_3||GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        if(m_ibuffer)
            m_ibuffer->unbind();
    }
}

}
}
}
