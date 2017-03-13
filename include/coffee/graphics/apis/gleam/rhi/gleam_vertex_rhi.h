#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_VertAttribute : GraphicsAPI::VertexAttribute
{
};

struct GLEAM_VertDescriptor : GraphicsAPI::VertexDescriptor
{
    void alloc();
    void dealloc();

    void addAttribute(GLEAM_VertAttribute const& attr);
    void bindBuffer(uint32 binding, GLEAM_ArrayBuffer &buf);
    void setIndexBuffer(GLEAM_ElementBuffer const*buffer);

    void bind();
    void unbind();
private:
    GLEAM_ElementBuffer const* m_ibuffer;
    Vector<GLEAM_VertAttribute> m_attributes;
    CGhnd m_handle;

#if defined(COFFEE_ONLY_GLES20)
    Map<uint32, GLEAM_ArrayBuffer&> m_bufferMapping;
#endif
};

}
}
}
