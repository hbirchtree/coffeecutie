#pragma once

#include "gleam_types_rhi.h"
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_VertAttribute : GraphicsAPI::VertexAttribute
{
};

struct GLEAM_VertDescriptor : GraphicsAPI::VertexDescriptor
{
    GLEAM_VertDescriptor():
        m_ibuffer(nullptr),
        m_attributes(),
    #if defined(COFFEE_ONLY_GLES20)
        m_bufferMapping()
    #else
        m_handle(0)
    #endif
    {

    }

    void alloc();
    void dealloc();

    void addAttribute(GLEAM_VertAttribute const& attr);
    void bindBuffer(uint32 binding, GLEAM_ArrayBuffer &buf);
    void setIndexBuffer(GLEAM_ElementBuffer const*buffer);

    void bind(u32 vertexOffset = 0);
    void unbind();
private:
    GLEAM_ElementBuffer const* m_ibuffer = nullptr;
    Vector<GLEAM_VertAttribute> m_attributes;

#if defined(COFFEE_ONLY_GLES20)
    Map<uint32, GLEAM_ArrayBuffer&> m_bufferMapping;
#else
    CGhnd m_handle;
#endif
};

}
}
}
