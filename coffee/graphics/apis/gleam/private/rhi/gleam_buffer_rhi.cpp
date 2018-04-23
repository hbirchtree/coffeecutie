#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {
STATICINLINE void VerifyBuffer(CGhnd h)
{
    if(GL_DEBUG_MODE && !CGL33::Debug::IsBuffer(h))
        cWarning("Invalid use of buffer API,"
                 " buffer handle is not valid");
}

void GLEAM_VBuffer::alloc()
{
#if defined(COFFEE_GLEAM_DESKTOP)
    if(GLEAM_FEATURES.direct_state)
        CGL45::BufAlloc(m_handle);
    else
#endif
        CGL33::BufAlloc(m_handle);
}

void GLEAM_VBuffer::dealloc()
{
    VerifyBuffer(m_handle);
    CGL33::BufFree(m_handle);
    m_handle = 0;
}

void GLEAM_VBuffer::commit(szptr size, c_cptr data)
{
    m_size = size;
    bind();
#if defined(COFFEE_GLEAM_DESKTOP)
    if(GLEAM_FEATURES.buffer_storage &&
       feval(m_access & ResourceAccess::Immutable))
    {
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufStorage(m_handle, Bytes::From(data, size), m_access);
        else
            CGL_BufferStorage<GLVER_44>::BufStorage(
                m_type, Bytes::From(data, size), m_access);
    } else
#endif
    {
#if defined(COFFEE_GLEAM_DESKTOP)
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufData(m_handle, Bytes::From(data, m_size), m_access);
        else
#endif
            CGL33::BufData(m_type, Bytes::From(data, m_size), m_access);
    }
}

void* GLEAM_VBuffer::map(szptr offset, szptr size)
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        VerifyBuffer(m_handle);

        if(size == 0 && offset == 0)
            size = m_size;
        if(offset + size > m_size)
            throw std::out_of_range(GLM_API "offset + size too big");

        bind();

        auto acc = m_access;
        if(!GLEAM_FEATURES.buffer_persistent)
            acc ^= ResourceAccess::Persistent;

#if defined(COFFEE_GLEAM_DESKTOP)
        if(GLEAM_FEATURES.direct_state)
            return CGL45::BufMapRange(
                m_handle, C_FCAST<ptroff>(offset), C_FCAST<ptroff>(size), acc);
        else
#endif
            return CGL33::BufMapRange(
                m_type,
                C_FCAST<ptroff>(offset),
                C_FCAST<ptroff>((size) ? size : m_size),
                acc);
    } else
#else
    C_UNUSED(offset);
#endif
    {
        // TODO: Fix cases where offset is used for something!!!
        m_mappedBufferFake.resize(size);
        return &m_mappedBufferFake[0];
    }
}

void GLEAM_VBuffer::unmap()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        VerifyBuffer(m_handle);

        bind();
#if defined(COFFEE_GLEAM_DESKTOP)
        if(GLEAM_FEATURES.direct_state)
            CGL45::BufUnmap(m_handle);
        else
#endif
            CGL33::BufUnmap(m_type);
    } else
#endif
    {
        commit(m_mappedBufferFake.size(), &m_mappedBufferFake[0]);
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
        CGL33::BufBind(m_type, 0);
}

void GLEAM_BindableBuffer::bindrange(uint32 idx, szptr off, szptr size) const
{
#if !defined(COFFEE_ONLY_GLES20)
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
        throw implementation_error("cannot bind ranges with this API");
    }
}

void GLEAM_PixelBuffer::setState(bool pack)
{
    m_type = (pack) ? BufType::PixelPData : BufType::PixelUData;
}
} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
