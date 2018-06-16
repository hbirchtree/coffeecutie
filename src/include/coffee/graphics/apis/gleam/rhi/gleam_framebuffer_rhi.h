#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_RenderDummy : GraphicsAPI::RenderDummy
{
    friend struct GLEAM_RenderTarget;

    void allocate(PixelFormat fmt, DBuffers buf, uint32 index, CSize size);
    void deallocate();

  protected:
    glhnd    m_handle;
    DBuffers m_type;
    uint32   m_attachment;
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

    void attachSurface(GLEAM_Surface const& s, uint32 idx, uint32 mip = 0);
    void attachSurface(GLEAM_RenderDummy const& rb);

    void attachDepthStencilSurface(GLEAM_Surface const& s, uint32 mip);
    void attachDepthSurface(GLEAM_Surface const& s, uint32 mip);

    void blit(
        CRect64 const&      src,
        GLEAM_RenderTarget& target,
        CRect64 const&      tgt,
        DBuffers            buf,
        Filtering           flt);

    void resize(uint32 i, CRect64 const& view);

    CSize size();

    void clear(uint32 i, Vecf4 const& color);
    void clear(bigscalar depth);
    void clear(bigscalar depth, int32 stencil);
    void clear(uint32 i, Vecf4 const& color, bigscalar depth);
    void clear(uint32 i, Vecf4 const& color, bigscalar depth, int32 stencil);

  protected:
    void bind(FramebufferT t);
    void unbind(FramebufferT t);
    bool validate() const;

    glhnd        m_handle;
    FramebufferT m_type;
    CSize        m_size;
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
