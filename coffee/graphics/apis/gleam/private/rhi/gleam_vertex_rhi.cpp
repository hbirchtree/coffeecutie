#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

using Lim = CGL_Shared_Limits;

static void vao_apply_buffer(
        Vector<GLEAM_VertAttribute> const& m_attributes,
        uint32 binding, GLEAM_ArrayBuffer&,
        u32 vertexOffset = 0)
{
    for(GLEAM_VertAttribute const& attr : m_attributes)
        if(binding == attr.bufferAssociation())
        {
            CGL33::VAOEnableAttrib(attr.index());
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
            if(use_integer
                    && !(attr.m_flags & GLEAM_API::AttributePacked)
                    && !GLEAM_FEATURES.gles20)
                CGL33::VAOAttribIPointer(
                            attr.index(),attr.size(),attr.type(),
                            attr.stride(),
                            attr.bufferOffset()
                            + attr.offset()
                            + vertexOffset * attr.stride()
                            );
            else
#endif
                CGL33::VAOAttribPointer(
                            attr.index(),attr.size(),attr.type(),
                            attr.m_flags & GLEAM_API::AttributeNormalization,
                            attr.stride(),
                            attr.bufferOffset()
                            + attr.offset()
                            + vertexOffset * attr.stride());

#if !defined(COFFEE_ONLY_GLES20)
            if(attr.instanced())
                CGL33::VAODivisor(attr.index(),1);
#endif
        }
}

void GLEAM_VertDescriptor::alloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        CGL33::VAOAlloc(1,&m_handle);
#endif
}

void GLEAM_VertDescriptor::dealloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        CGL33::VAOFree(1,&m_handle);
    m_handle = 0;
#endif
}

void GLEAM_VertDescriptor::addAttribute(const GLEAM_VertAttribute &attr)
{
    m_attributes.push_back(attr);
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::VAOBind(m_handle);
        CGL33::VAOEnableAttrib(attr.index());

        if(GLEAM_FEATURES.vertex_format)
        {
            CGL43::VAOAttribFormat(
                        attr.index(),attr.size(),
                        attr.type(),
                        attr.m_flags & GLEAM_API::AttributeNormalization,
                        attr.offset());
        }
    }
#endif
}

void GLEAM_VertDescriptor::bindBuffer(uint32 binding, GLEAM_ArrayBuffer &buf)
{
    if(GLEAM_FEATURES.gles20)
        m_bufferMapping.insert({binding, buf});
#if !defined(COFFEE_ONLY_GLES20)
    else
    {
        CGL43::VAOBind(m_handle);
        if(!GLEAM_FEATURES.vertex_format)
        {
            buf.bind();
            vao_apply_buffer(m_attributes, binding, buf);
        }
        else if(GLEAM_FEATURES.vertex_format)
        {
            for(GLEAM_VertAttribute const& attr : m_attributes)
                if(binding == attr.bufferAssociation())
                {
                    CGL43::VAOBindVertexBuffer(
                                binding,buf.m_handle,
                                attr.bufferOffset(),
                                attr.stride());
                    CGL43::VAOAttribBinding(attr.index(),binding);
                    if(attr.instanced())
                        CGL43::VAOBindingDivisor(attr.index(),1);
                }
        }
    }
#endif
}

void GLEAM_VertDescriptor::setIndexBuffer(const GLEAM_ElementBuffer *buffer)
{
    m_ibuffer = buffer;
}

void GLEAM_VertDescriptor::bind(u32 vertexOffset)
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::VAOBind(m_handle);
        C_UNUSED(vertexOffset);
    }else
#endif
        for(Pair<uint32, GLEAM_ArrayBuffer&> binding : m_bufferMapping)
        {
            binding.second.bind();
            vao_apply_buffer(m_attributes, binding.first, binding.second,
                             vertexOffset);
        }

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        m_ibuffer->bind();
}

void GLEAM_VertDescriptor::unbind()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        CGL33::VAOBind(0);
    else
#endif
        for(auto const& attr : m_attributes)
            CGL33::VAODisableAttrib(attr.index());

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        m_ibuffer->unbind();
}

}
}
}
