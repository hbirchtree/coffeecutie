#pragma once

#include "gleam_types_rhi.h"
#include "gleam_framebuffer_rhi.h"
#include "gleam_surface_rhi.h"

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

    GLEAM_Surface2D m_depth_stencil;
    GLEAM_Surface2D m_color;

    GLEAM_Sampler2D m_depth_stencil_sampler;
    GLEAM_Sampler2D m_color_sampler;

    CGhnd m_vbo;
    CGhnd m_vao;
    CGhnd m_prg;

    int32 m_tex_size_unif;
    int32 m_trans_unif;
    int32 m_tex_unif;

    bool m_enabled;
};

}
}
}
