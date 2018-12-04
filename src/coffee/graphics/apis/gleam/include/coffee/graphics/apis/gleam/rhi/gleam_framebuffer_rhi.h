#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_RenderDummy : GraphicsAPI::RenderDummy
{
    friend struct GLEAM_RenderTarget;

    void allocate(PixFmt fmt, DBuffers buf, u32 index, Size size);
    void deallocate();

  protected:
    glhnd    m_handle;
    DBuffers m_type;
    u32      m_attachment;
};

struct GLEAM_RenderTarget : GraphicsAPI::RenderTarget
{
    friend struct GLEAM_DBufQuery;

    GLEAM_RenderTarget() : GLEAM_RenderTarget(FramebufferT::All)
    {
    }
    GLEAM_RenderTarget(FramebufferT t) : GraphicsAPI::RenderTarget(), m_type(t)
    {
    }

    void use(FramebufferT t)
    {
        validate();
        bind(t);
    }

    void alloc();
    void dealloc();

    /* TODO: Layered attachments */

    void attachSurface(GLEAM_Surface2D const& s, u32 idx, u32 mip = 0);
    void attachSurface(GLEAM_RenderDummy const& rb);

    void attachDepthStencilSurface(GLEAM_Surface2D const& s, u32 mip);
    void attachDepthSurface(GLEAM_Surface2D const& s, u32 mip);

    void blit(
        Rect64 const&      src,
        GLEAM_RenderTarget& target,
        Rect64 const&      tgt,
        DBuffers            buf,
        Filtering           flt);

    void resize(u32 i, Rect64 const& view);

    Size size();

    void clear(u32 i, Vecf4 const& color);
    void clear(bigscalar depth);
    void clear(bigscalar depth, i32 stencil);
    void clear(u32 i, Vecf4 const& color, bigscalar depth);
    void clear(u32 i, Vecf4 const& color, bigscalar depth, i32 stencil);

  protected:
    void bind(FramebufferT t);
    void unbind(FramebufferT t);
    bool validate() const;

    glhnd        m_handle;
    FramebufferT m_type;
    Size        m_size;
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
