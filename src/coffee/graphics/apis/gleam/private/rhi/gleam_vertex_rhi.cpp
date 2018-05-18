#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include "gleam_internal_types.h"
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>

namespace Coffee {
namespace RHI {
namespace GLEAM {

using Lim = CGL_Shared_Limits;

static bool IsIntegerType(TypeEnum e)
{
    switch(e)
    {
    case TypeEnum::Byte:
    case TypeEnum::UByte:
    case TypeEnum::Short:
    case TypeEnum::UShort:
    case TypeEnum::Int:
    case TypeEnum::UInt:
    case TypeEnum::LL:
    case TypeEnum::ULL:
        return true;
    default:
        return false;
    }
}

/*!
 * \brief vao_apply_buffer
 * \param m_attributes
 * \param binding
 * \param vertexOffset
 */
static void vao_apply_buffer(
    Vector<GLEAM_VertAttribute> const& m_attributes,
    uint32                             binding,
    GLEAM_ArrayBuffer&,
    u32 vertexOffset = 0)
{
    for(GLEAM_VertAttribute const& attr : m_attributes)
        if(binding == attr.bufferAssociation())
        {
            CGL33::VAOEnableAttrib(attr.index());
#if GL_VERSION_VERIFY(0x300, 0x300)
            bool use_integer = IsIntegerType(attr.type());

            if(use_integer && !(attr.m_flags & GLEAM_API::AttributePacked) &&
               !GLEAM_FEATURES.gles20)
                CGL33::VAOAttribIPointer(
                    attr.index(),
                    attr.size(),
                    attr.type(),
                    attr.stride(),
                    attr.bufferOffset() + attr.offset() +
                        vertexOffset * attr.stride());
            else
#endif
                CGL33::VAOAttribPointer(
                    attr.index(),
                    attr.size(),
                    attr.type(),
                    attr.m_flags & GLEAM_API::AttributeNormalization,
                    attr.stride(),
                    attr.bufferOffset() + attr.offset() +
                        vertexOffset * attr.stride());

#if GL_VERSION_VERIFY(0x330, 0x300)
            if(attr.instanced())
                CGL33::VAODivisor(attr.index(), 1);
#endif
        }
}

void GLEAM_VertDescriptor::alloc()
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        CGL45::VAOAlloc(m_handle);
    else
#if GL_VERSION_VERIFY(0x300, 0x300)
        if(!GLEAM_FEATURES.gles20)
        CGL33::VAOAlloc(m_handle);
#endif
#endif
}

void GLEAM_VertDescriptor::dealloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        CGL33::VAOFree(m_handle);
    m_handle = 0;
#endif
}

void GLEAM_VertDescriptor::addAttribute(const GLEAM_VertAttribute& attr)
{
    m_attributes.push_back(attr);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(!GLEAM_FEATURES.gles20)
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
        {
            CGL45::VAOEnableAttrib(m_handle, attr.index());

            if(IsIntegerType(attr.type()) &&
               !(attr.m_flags & GLEAM_API::AttributePacked))
                CGL45::VAOAttribFormatI(
                    m_handle,
                    attr.index(),
                    attr.size(),
                    attr.type(),
                    attr.offset());
            else
                CGL45::VAOAttribFormat(
                    m_handle,
                    attr.index(),
                    attr.size(),
                    attr.type(),
                    attr.m_flags & GLEAM_API::AttributeNormalization,
                    attr.offset());
        } else
#endif
        {
            CGL33::VAOBind(m_handle);
            CGL33::VAOEnableAttrib(attr.index());

            if(GLEAM_FEATURES.vertex_format)
            {
                if(IsIntegerType(attr.type()) &&
                   !(attr.m_flags & GLEAM_API::AttributePacked))
                    CGL43::VAOAttribFormatI(
                        attr.index(), attr.size(), attr.type(), attr.offset());
                else
                    CGL43::VAOAttribFormat(
                        attr.index(),
                        attr.size(),
                        attr.type(),
                        attr.m_flags & GLEAM_API::AttributeNormalization,
                        attr.offset());
            }
        }
    }
#endif
}

void GLEAM_VertDescriptor::bindBuffer(uint32 binding, GLEAM_ArrayBuffer& buf)
{
    if(GLEAM_FEATURES.gles20)
    {
        m_bufferMapping.insert({binding, buf});
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(!GLEAM_FEATURES.direct_state)
#endif
            CGL43::VAOBind(m_handle);

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
        {
            for(GLEAM_VertAttribute const& attr : m_attributes)
            {
                if(binding == attr.bufferAssociation())
                {
                    CGL45::VAOBindVertexBuffer(
                        m_handle,
                        binding,
                        buf.m_handle,
                        attr.bufferOffset(),
                        attr.stride());
                    CGL45::VAOAttribBinding(
                        m_handle, attr.index(), attr.bufferAssociation());
                    if(attr.instanced())
                        CGL45::VAOBindingDivisor(m_handle, binding, 1);
                }
            }
        } else
#endif
            if(GLEAM_FEATURES.vertex_format)
        {
            for(GLEAM_VertAttribute const& attr : m_attributes)
                if(binding == attr.bufferAssociation())
                {
                    CGL43::VAOBindVertexBuffer(
                        binding,
                        buf.m_handle,
                        attr.bufferOffset(),
                        attr.stride());
                    CGL43::VAOAttribBinding(attr.index(), binding);
                    if(attr.instanced())
                        CGL43::VAOBindingDivisor(attr.index(), 1);
                }
        } else
        {
            buf.bind();
            vao_apply_buffer(m_attributes, binding, buf);
        }
    }
#endif
}

void GLEAM_VertDescriptor::setIndexBuffer(const GLEAM_ElementBuffer* buffer)
{
    m_ibuffer = buffer;

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.vertex_format && GLEAM_FEATURES.direct_state)
        CGL45::VAOElementBuffer(m_handle, buffer->m_handle);
#endif
}

void GLEAM_VertDescriptor::bind(u32 vertexOffset)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::VAOBind(m_handle);
        C_UNUSED(vertexOffset);
    } else
#endif
        for(Pair<uint32, GLEAM_ArrayBuffer&> binding : m_bufferMapping)
        {
            binding.second.bind();
            vao_apply_buffer(
                m_attributes, binding.first, binding.second, vertexOffset);
        }

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        CGL33::BufBind(m_ibuffer->m_type, m_ibuffer->m_handle);
}

void GLEAM_VertDescriptor::unbind()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        CGL33::VAOBind(0);
    else
#endif
        for(auto const& attr : m_attributes)
            CGL33::VAODisableAttrib(attr.index());

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        m_ibuffer->unbind();
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
