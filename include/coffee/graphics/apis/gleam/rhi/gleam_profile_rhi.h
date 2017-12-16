#pragma once

#include "gleam_types_rhi.h"
#include "gleam_framebuffer_rhi.h"
#include "gleam_surface_rhi.h"
#include "gleam_query_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_PrfQuery : GraphicsProfiler::PerfQuery, GLEAM_Query
{
    GLEAM_PrfQuery(ProfilingTerm term);

    void begin();
    void end();

    int64 resulti();
    uint64 resultu();
};

struct GLEAM_DBufQuery : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>
{
    GLEAM_DBufQuery(GLEAM_RenderTarget& t, DBuffers b);
    ~GLEAM_DBufQuery();

    void resize(CSize const& s);

    void begin();
    void end();
protected:
#ifndef NDEBUG
    CSize m_size;

    GLEAM_Surface2D m_depth_stencil;
    GLEAM_Surface2D m_color;

    GLEAM_Sampler2D m_depth_stencil_sampler;
    GLEAM_Sampler2D m_color_sampler;

    bool m_enabled;
#endif
};

struct GLEAM_ScopeMarker : GraphicsDebug::ScopeMarker
{
    GLEAM_ScopeMarker(cstring tag);

    ~GLEAM_ScopeMarker();
};

}
}
}
