#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_VBuffer : GraphicsAPI::VertexBuffer
{
    friend struct GLEAM_VertDescriptor;
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface2DArray;

    GLEAM_VBuffer(BufType type, ResourceAccess acc, szptr size):
        VertexBuffer(acc,size),
        m_type(type),
        m_handle(0)
    {
    }

    void alloc();
    void dealloc();

    void commit(szptr size, c_cptr data = nullptr);

    void* map(szptr offset = 0,szptr size = 0);
    void unmap();

protected:
    void bind() const;
    void unbind() const;

    BufType m_type;
    CGhnd m_handle;

#if defined(COFFEE_ONLY_GLES20)
    Vector<byte_t> m_mappedBufferFake;
#endif
};

struct GLEAM_BindableBuffer : GLEAM_VBuffer
{
    GLEAM_BindableBuffer(BufType type,ResourceAccess acc, szptr size):
        GLEAM_VBuffer(type,acc,size)
    {
    }
    void bindrange(uint32 idx, szptr off, szptr size) const;
};

struct GLEAM_UniformBuffer : GLEAM_BindableBuffer
{
    GLEAM_UniformBuffer(ResourceAccess acc, szptr size):
        GLEAM_BindableBuffer(BufType::UniformData,acc,size)
    {
    }
};

struct GLEAM_ShaderBuffer : GLEAM_BindableBuffer
{
    GLEAM_ShaderBuffer(ResourceAccess acc, szptr size):
        GLEAM_BindableBuffer(BufType::ShaderData,acc,size)
    {
    }
};

struct GLEAM_ArrayBuffer : GLEAM_VBuffer
{
    GLEAM_ArrayBuffer(ResourceAccess acc,szptr size):
        GLEAM_VBuffer(BufType::ArrayData,acc,size)
    {
    }
};

struct GLEAM_ElementBuffer : GLEAM_VBuffer
{
    GLEAM_ElementBuffer(ResourceAccess acc, szptr size):
        GLEAM_VBuffer(BufType::ElementData,acc,size)
    {
    }
};

struct GLEAM_PixelBuffer : GLEAM_VBuffer
{
    GLEAM_PixelBuffer(ResourceAccess acc, szptr size):
        GLEAM_VBuffer(BufType::PixelUData,acc,size)
    {
    }

    void setState(bool pack);
};

}
}
}
