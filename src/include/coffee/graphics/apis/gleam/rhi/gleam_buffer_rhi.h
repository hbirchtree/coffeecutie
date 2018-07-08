#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_VBuffer : GraphicsAPI::VertexBuffer
{
    friend struct GLEAM_VertDescriptor;
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface2DArray;

    GLEAM_VBuffer(BufType type, RSCA acc, szptr) :
        VertexBuffer(acc), m_type(type), m_handle(0)
    {
    }

    void alloc();
    void dealloc();

    void commit(BytesConst const& data);
    void commit(szptr size, c_cptr data = nullptr)
    {
        commit(BytesConst::Unsafe(data, size));
    }

    Bytes map(szptr offset, szptr size, gleam_error& ec);
    Bytes map(szptr offset = 0, szptr size = 0)
    {
        gleam_error ec;
        return map(offset, size, ec);
    }
    void unmap();

  protected:
    void bind() const;
    void unbind() const;

    BufType m_type;
    glhnd   m_handle;

    Vector<byte_t> m_mappedBufferFake;
};

struct GLEAM_BindableBuffer : GLEAM_VBuffer
{
    GLEAM_BindableBuffer(BufType type, RSCA acc, szptr stride, szptr size) :
        GLEAM_VBuffer(type, acc, size), m_stride(stride)
    {
    }
    void bindrange(uint32 idx, szptr off, szptr size, gleam_error& ec) const;

  protected:
    szptr m_stride;
};

struct GLEAM_UniformBuffer : GLEAM_BindableBuffer
{
    GLEAM_UniformBuffer(RSCA acc, szptr stride, szptr size) :
        GLEAM_BindableBuffer(buf::constants_ro::value, acc, stride, size)
    {
    }
};

struct GLEAM_ShaderBuffer : GLEAM_BindableBuffer
{
    GLEAM_ShaderBuffer(RSCA acc, szptr stride, szptr size) :
        GLEAM_BindableBuffer(buf::constants_rw::value, acc, stride, size)
    {
    }
};

struct GLEAM_ArrayBuffer : GLEAM_VBuffer
{
    GLEAM_ArrayBuffer(RSCA acc, szptr size) :
        GLEAM_VBuffer(buf::vertex::value, acc, size)
    {
    }
};

struct GLEAM_ElementBuffer : GLEAM_VBuffer
{
    GLEAM_ElementBuffer(RSCA acc, szptr size) :
        GLEAM_VBuffer(buf::elements::value, acc, size)
    {
    }
};

struct GLEAM_PixelBuffer : GLEAM_VBuffer
{
    GLEAM_PixelBuffer(RSCA acc, szptr size) :
        GLEAM_VBuffer(buf::pixel::value, acc, size)
    {
    }

    void setState(bool pack);
};

struct GLEAM_IndirectBuffer : GLEAM_VBuffer
{
    GLEAM_IndirectBuffer(RSCA acc, u32 flags, szptr stride, szptr size) :
        GLEAM_VBuffer(buf::draw_indirect::value, acc, size), m_stride(stride),
        m_flags(flags)

    {
    }

  protected:
    szptr m_stride;
    u32   m_flags;
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
