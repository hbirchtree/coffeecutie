#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {
STATICINLINE void VerifyBuffer(glhnd const& h)
{
    // TODO: Return error

    if(GL_DEBUG_MODE && !CGL::Debug::IsBuffer(h.hnd))
        return;
//        cWarning("Invalid use of buffer API,"
//                 " buffer handle is not valid");
}

void GLEAM_VBuffer::alloc()
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        CGL45::BufAllocEx(m_handle.hnd);
    else
#endif
        CGL33::BufAlloc(m_handle.hnd);
}

void GLEAM_VBuffer::dealloc()
{
    VerifyBuffer(m_handle);
    CGL33::BufFree(m_handle.hnd);
    m_handle.release();
}

void GLEAM_VBuffer::commit(BytesConst const& data)
{
    m_size = data.size;
    bind();

#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
    if(GLEAM_FEATURES.buffer_storage && feval(m_access & RSCA::Immutable))
    {
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufStorage(m_handle, data, m_access);
        else
            CGL_44<GLVER_44>::BufStorage(m_type, data, m_access);
    } else
#endif
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufData(m_handle, data, m_access);
        else
#endif
            CGL33::BufData(m_type, data, m_access);
    }
}

Bytes GLEAM_VBuffer::map(C_UNUSED(szptr offset), szptr size, gleam_error& ec)
{
    void* out_ptr = nullptr;

    if(!size)
    {
        if(offset)
            size = m_size - offset;
        else
            size = m_size;
    }

    if(offset + size > m_size)
    {
        ec = APIE::BufferMappingOutOfBounds;
        return {};
    }

#if GL_VERSION_VERIFY(0x330, 0x300) && !defined(COFFEE_WEBGL)
    if(!GLEAM_FEATURES.gles20)
    {
        VerifyBuffer(m_handle);

        bind();

        auto acc = m_access;
        if(!GLEAM_FEATURES.buffer_persistent)
            acc ^= RSCA::Persistent;

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
            out_ptr = CGL45::BufMapRange(
                m_handle, C_FCAST<ptroff>(offset), C_FCAST<ptroff>(size), acc);
        else
#endif
            out_ptr = CGL33::BufMapRange(
                m_type, C_FCAST<ptroff>(offset), C_FCAST<ptroff>(size), acc);
    } else
#endif
    {
        // TODO: Fix cases where offset is used for something!!!
        m_mappedBufferFake.resize(size);
        return m_mappedBufferFake;
    }

    if(!out_ptr)
    {
        ec = APIE::BufferMappingFailed;
        return {};
    }

    return Bytes::From(out_ptr, size);
}

void GLEAM_VBuffer::unmap()
{
#if GL_VERSION_VERIFY(0x330, 0x300) && !defined(COFFEE_WEBGL)
    if(!GLEAM_FEATURES.gles20)
    {
        VerifyBuffer(m_handle);

        bind();
#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufUnmap(m_handle);
        else
#endif
            CGL33::BufUnmap(m_type);
        return;
    } else
#endif
    {
        commit(m_mappedBufferFake.size(), &m_mappedBufferFake[0]);
        return;
    }
}

void GLEAM_VBuffer::bind() const
{
    if(!GLEAM_FEATURES.direct_state)
        CGL33::BufBind(m_type, m_handle);
}

void GLEAM_VBuffer::unbind() const
{
    if(!GLEAM_FEATURES.direct_state)
        CGL33::BufBind(m_type, glhnd());
}

void GLEAM_BindableBuffer::bindrange(
    u32 idx, szptr off, szptr size, gleam_error& ec) const
{
#if GL_VERSION_VERIFY(0x330, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        VerifyBuffer(m_handle);

        CGL33::BufBindRange(
            m_type, idx, m_handle, C_FCAST<ptroff>(off), C_FCAST<ptroff>(size));
    } else
#else
    C_USED(idx);
    C_USED(off);
    C_USED(size);
#endif
    {
        ec = APIE::UnimplementedPath;
    }
}

void GLEAM_PixelBuffer::setState(bool pack)
{
    m_type = (pack) ? buf::pixel_pack::value : buf::pixel_unpack::value;
}
} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
