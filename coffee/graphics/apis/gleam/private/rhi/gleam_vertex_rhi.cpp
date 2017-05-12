#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

static void vao_apply_buffer(Vector<GLEAM_VertAttribute> const& m_attributes,
                             uint32 binding, GLEAM_ArrayBuffer&)
{
    for(GLEAM_VertAttribute const& attr : m_attributes)
        if(binding == attr.bufferAssociation())
        {
#if !defined(COFFEE_ONLY_GLES20)
            bool use_integer = false;
            switch(attr.type())
            {
            case TypeEnum::Byte:
            case TypeEnum::UByte:
            case TypeEnum::Short:
            case TypeEnum::UShort:
            case TypeEnum::Int:
            case TypeEnum::UInt:
            case TypeEnum::LL:
            case TypeEnum::ULL:
                use_integer = true;
                break;
            default:
                break;
            }
            if(use_integer && !(attr.m_flags & GLEAM_API::AttributePacked))
                CGL33::VAOAttribIPointer(
                            attr.index(),attr.size(),attr.type(),
                            attr.stride(),
                            attr.bufferOffset()+attr.offset());
            else
#endif
                CGL33::VAOAttribPointer(
                            attr.index(),attr.size(),attr.type(),
                            attr.m_flags & GLEAM_API::AttributeNormalization,
                            attr.stride(),
                            attr.bufferOffset()+attr.offset());

#if !defined(COFFEE_ONLY_GLES20)
            if(attr.instanced())
                CGL33::VAODivisor(attr.index(),1);
#endif
        }
}

void GLEAM_VertDescriptor::alloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOAlloc(1,&m_handle);
#endif
}

void GLEAM_VertDescriptor::dealloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOFree(1,&m_handle);
#endif
}

void GLEAM_VertDescriptor::addAttribute(const GLEAM_VertAttribute &attr)
{
    m_attributes.push_back(attr);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOBind(m_handle);
#endif
    CGL33::VAOEnableAttrib(attr.index());
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        CGL43::VAOAttribFormat(attr.index(),attr.size(),attr.type(),false,attr.offset());
    }
#endif
}

void GLEAM_VertDescriptor::bindBuffer(uint32 binding, GLEAM_ArrayBuffer &buf)
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL43::VAOBind(m_handle);
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        buf.bind();
        vao_apply_buffer(m_attributes, binding, buf);
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
#else
    m_bufferMapping.insert({binding, buf});
#endif
}

void GLEAM_VertDescriptor::setIndexBuffer(const GLEAM_ElementBuffer *buffer)
{
    m_ibuffer = buffer;
}

void GLEAM_VertDescriptor::bind()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOBind(m_handle);
#else
    for(Pair<uint32, GLEAM_ArrayBuffer&> binding : m_bufferMapping)
    {
        binding.second.bind();
        vao_apply_buffer(m_attributes, binding.first, binding.second);
    }
#endif
    if(GL_CURR_API==GL_3_3||GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_2_0
            || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        if(m_ibuffer)
            m_ibuffer->bind();
    }
}

void GLEAM_VertDescriptor::unbind()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOBind(0);
#endif
    if(GL_CURR_API==GL_3_3||GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_2_0
            || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        if(m_ibuffer)
            m_ibuffer->unbind();
    }
}

}
}
}
