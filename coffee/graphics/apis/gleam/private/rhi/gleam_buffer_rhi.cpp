#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

STATICINLINE void VerifyBuffer(CGhnd h)
{
    if(GL_DEBUG_MODE)
        if(!CGL33::Debug::IsBuffer(h))
            cWarning("Invalid use of buffer API, buffer handle is not valid");
}

void GLEAM_VBuffer::alloc()
{
    CGL33::BufAlloc(1,&m_handle);
}

void GLEAM_VBuffer::dealloc()
{
    VerifyBuffer(m_handle);
    CGL33::BufFree(1,&m_handle);
}

void GLEAM_VBuffer::commit(szptr size, c_cptr data)
{
    m_size = size;
    bind();
    if(GL_CURR_API==GL_3_3|| GL_CURR_API==GLES_2_0 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
        CGL33::BufData(m_type,m_size,data,m_access);
#if !defined(COFFEE_ONLY_GLES20)
    else if(GL_CURR_API==GL_4_3)
        CGL43::BufStorage(m_type,m_size,data,m_access);
#endif
}

void *GLEAM_VBuffer::map(szptr offset,szptr size)
{
#if !defined(COFFEE_ONLY_GLES20)
    VerifyBuffer(m_handle);

    if(size == 0 && offset == 0)
        size = m_size;
    if(offset+size > m_size)
        return nullptr;
    bind();

    auto acc = m_access;
    if(GL_CURR_API == GL_3_3)
        acc ^= ResourceAccess::Persistent;

    return CGL33::BufMapRange(m_type,offset,(size) ? size : m_size,acc);
#else
    m_mappedBufferFake.resize(size);
    return &m_mappedBufferFake[0];
#endif
}

void GLEAM_VBuffer::unmap()
{
#if !defined(COFFEE_ONLY_GLES20)
    VerifyBuffer(m_handle);

    bind();
    CGL33::BufUnmap(m_type);
#else
    commit(m_mappedBufferFake.size(), &m_mappedBufferFake[0]);
#endif
}

void GLEAM_VBuffer::bind() const
{
    CGL33::BufBind(m_type,m_handle);
}

void GLEAM_VBuffer::unbind() const
{
    CGL33::BufBind(m_type,0);
}

void GLEAM_BindableBuffer::bindrange(uint32 idx, szptr off, szptr size) const
{
#if !defined(COFFEE_ONLY_GLES20)
    VerifyBuffer(m_handle);

    CGL33::BufBindRange(m_type,idx,m_handle,off,size);
#endif
}

void GLEAM_PixelBuffer::setState(bool pack)
{
    m_type = (pack) ? BufType::PixelPData : BufType::PixelUData;
}

}
}
}
