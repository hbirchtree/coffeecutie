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
    u32                             binding,
    GLEAM_ArrayBuffer&,
    u32 vertexOffset = 0)
{
    for(GLEAM_VertAttribute const& attr : m_attributes)
        if(binding == attr.bufferAssociation())
        {
            gl::v33::VAOEnableArray(attr.index());
#if GL_VERSION_VERIFY(0x300, 0x300)
            bool use_integer = IsIntegerType(attr.type());

            if(use_integer && !(attr.m_flags & GLEAM_API::AttributePacked) &&
               !GLEAM_FEATURES.gles20)
                gl::v33::VAOIPointer(
                    attr.index(),
                    attr.size(),
                    to_enum(attr.type()),
                    attr.stride(),
                    C_RCAST<c_ptr>(
                        attr.bufferOffset() + attr.offset() +
                        vertexOffset * attr.stride()));
            else
#endif
                gl::v33::VAOPointer(
                    attr.index(),
                    attr.size(),
                    to_enum(attr.type()),
                    attr.m_flags & GLEAM_API::AttributeNormalization,
                    attr.stride(),
                    C_RCAST<c_ptr>(
                        attr.bufferOffset() + attr.offset() +
                        vertexOffset * attr.stride()));

#if GL_VERSION_VERIFY(0x330, 0x300)
            if(attr.instanced())
                gl::v33::VAODivisor(attr.index(), 1);
#endif
        }
}

void GLEAM_VertDescriptor::alloc()
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        gl::v45::VAOAllocEx(m_handle.hnd);
    else
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
        if(!GLEAM_FEATURES.gles20)
        gl::v33::VAOAlloc(m_handle.hnd);
#endif
}

void GLEAM_VertDescriptor::dealloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        gl::v33::VAOFree(m_handle.hnd);
    m_handle.release();
#endif
}

void GLEAM_VertDescriptor::addAttribute(const GLEAM_VertAttribute& attr)
{
    m_attributes.push_back(attr);
#if GL_VERSION_VERIFY(0x330, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
        {
            gl::v45::VAOEnable(m_handle, attr.index());

            if(IsIntegerType(attr.type()) &&
               !(attr.m_flags & GLEAM_API::AttributePacked))
                gl::v45::VAOIFormat(
                    m_handle,
                    attr.index(),
                    attr.size(),
                    to_enum(attr.type()),
                    attr.offset());
            else
                gl::v45::VAOFormat(
                    m_handle,
                    attr.index(),
                    attr.size(),
                    to_enum(attr.type()),
                    attr.m_flags & GLEAM_API::AttributeNormalization,
                    attr.offset());
        } else
#endif
        {
            gl::v33::VAOBind(m_handle);
            gl::v33::VAOEnableArray(attr.index());

#if GL_VERSION_VERIFY(0x430, 0x310)
            if(GLEAM_FEATURES.vertex_format)
            {
                if(IsIntegerType(attr.type()) &&
                   !(attr.m_flags & GLEAM_API::AttributePacked))
                    gl::v43::VAOIFormat(
                        attr.index(),
                        attr.size(),
                        to_enum(attr.type()),
                        attr.offset());
                else
                    gl::v43::VAOFormat(
                        attr.index(),
                        attr.size(),
                        to_enum(attr.type()),
                        attr.m_flags & GLEAM_API::AttributeNormalization,
                        attr.offset());
            }
#endif
        }
    }
#endif
}

void GLEAM_VertDescriptor::bindBuffer(u32 binding, GLEAM_ArrayBuffer& buf)
{
    if(GLEAM_FEATURES.gles20)
    {
        m_bufferMapping.insert({binding, buf});
    }
#if GL_VERSION_VERIFY(0x330, 0x300)
    else
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(!GLEAM_FEATURES.direct_state)
#endif
            gl::v43::VAOBind(m_handle);

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
        {
            for(GLEAM_VertAttribute const& attr : m_attributes)
            {
                if(binding == attr.bufferAssociation())
                {
                    gl::v45::VAOVertexBuffer(
                        m_handle,
                        binding,
                        buf.m_handle,
                        attr.bufferOffset(),
                        attr.stride());
                    gl::v45::VAOBinding(
                        m_handle, attr.index(), attr.bufferAssociation());
                    if(attr.instanced())
                        gl::v45::VAOBindingDivisor(m_handle, binding, 1);
                }
            }
        } else
#endif
#if GL_VERSION_VERIFY(0x430, 0x310)
            if(GLEAM_FEATURES.vertex_format)
        {
            for(GLEAM_VertAttribute const& attr : m_attributes)
                if(binding == attr.bufferAssociation())
                {
                    gl::v43::VertBufBind(
                        binding,
                        buf.m_handle,
                        attr.bufferOffset(),
                        attr.stride());
                    gl::v43::VAOBinding(attr.index(), binding);
                    if(attr.instanced())
                        gl::v43::VAOBindingDivisor(attr.index(), 1);
                }
        } else
#endif
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
        gl::v45::VAOElementBuffer(m_handle, buffer->m_handle);
#endif
}

void GLEAM_VertDescriptor::bind(C_UNUSED(u32 vertexOffset))
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        gl::v33::VAOBind(m_handle);
    } else
#endif
    {
        for(Pair<u32, GLEAM_ArrayBuffer&> binding : m_bufferMapping)
        {
            binding.second.bind();
            vao_apply_buffer(
                m_attributes, binding.first, binding.second, vertexOffset);
        }
    }

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        gl::v33::BufBind(m_ibuffer->m_type, m_ibuffer->m_handle);
}

void GLEAM_VertDescriptor::unbind()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        gl::v33::VAOBind(glhnd());
    else
#endif
        for(auto const& attr : m_attributes)
            gl::v33::VAODisableArray(attr.index());

    if(GLEAM_FEATURES.element_buffer_bind && m_ibuffer)
        m_ibuffer->unbind();
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
