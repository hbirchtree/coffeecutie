#pragma once

#include "gleam_framebuffer_rhi.h"
#include "gleam_query_rhi.h"
#include "gleam_surface_rhi.h"
#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_PrfQuery : GraphicsProfiler::PerfQuery, GLEAM_Query
{
    GLEAM_PrfQuery(ProfilingTerm term);

    void begin();
    void end();

    i64 resulti();
    u64 resultu();
};

struct GLEAM_DBufQuery : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>
{
    GLEAM_DBufQuery(GLEAM_RenderTarget& t, DBuffers b);
    ~GLEAM_DBufQuery();

    void resize(Size const& s);

    void begin();
    void end();

  protected:
#if MODE_DEBUG
    Size m_size;

    GLEAM_Surface2D m_depth_stencil;
    GLEAM_Surface2D m_color;

    GLEAM_Sampler2D m_depth_stencil_sampler;
    GLEAM_Sampler2D m_color_sampler;

    bool m_enabled;
#endif
};

struct GLEAM_ScopeMarker : GraphicsDebug::ScopeMarker
{
    explicit GLEAM_ScopeMarker(CString const tag) :
        GLEAM_ScopeMarker(tag.c_str())
    {
    }
    GLEAM_ScopeMarker(cstring tag);

    ~GLEAM_ScopeMarker();
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
