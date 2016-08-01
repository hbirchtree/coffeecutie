#pragma once

#include "gleam_types_rhi.h"
#include "gleam_framebuffer_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_PrfQuery : GraphicsProfiler::PerfQuery
{
};

struct GLEAM_DBufQuery : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>
{
    GLEAM_DBufQuery(GLEAM_RenderTarget& t, DBuffers b);
    ~GLEAM_DBufQuery();

    void begin();
    void end();
protected:
    GLEAM_RenderTarget m_debug_target;

    GLEAM_RenderDummy m_depth_stencil;
    GLEAM_RenderDummy m_color;
};

}
}
}
