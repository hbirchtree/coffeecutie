#include <coffee/graphics_apis/gleam/rhi/gleam_buffer_rhi.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_VBuffer::alloc()
{
    CGL33::BufAlloc(1,&m_handle);
}

void GLEAM_VBuffer::dealloc()
{
    CGL33::BufFree(1,&m_handle);
}

void GLEAM_VBuffer::commit(szptr size, c_cptr data)
{
    m_size = size;
    CGL33::BufBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
        CGL33::BufData(m_type,m_size,data,m_access);
    else if(GL_CURR_API==GL_4_3)
        CGL43::BufStorage(m_type,m_size,data,m_access);
}

void *GLEAM_VBuffer::map(szptr offset,szptr size)
{
    if(size == 0 && offset == 0)
        size = m_size;
    if(offset+size > m_size)
        return nullptr;
    CGL33::BufBind(m_type,m_handle);
    return CGL33::BufMapRange(m_type,offset,(size) ? size : m_size,m_access);
}

void GLEAM_VBuffer::unmap()
{
    CGL33::BufBind(m_type,m_handle);
    CGL33::BufUnmap(m_type);
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
    CGL33::BufBindRange(m_type,idx,m_handle,off,size);
}

void GLEAM_PixelBuffer::setState(bool pack)
{
    m_type = (pack) ? BufType::PixelPData : BufType::PixelUData;
}

}
}
}
