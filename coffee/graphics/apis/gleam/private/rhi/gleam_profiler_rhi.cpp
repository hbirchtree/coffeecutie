#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_framebuffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_quad_draw.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

QueryT from_term(ProfilingTerm t)
{
    switch(t)
    {
    case ProfilingTerm::ElapsedTime:
        return QueryT::TimeElapsed;
    case ProfilingTerm::ScreenPrimitives:
        return QueryT::PrimGen;
    case ProfilingTerm::ScreenSamples:
        return QueryT::Samples;
    case ProfilingTerm::TransformedPrimitives:
        return QueryT::XFGen;
    default:
        return QueryT::AnySamples;
    }
}

GLEAM_PrfQuery::GLEAM_PrfQuery(ProfilingTerm term):
    PerfQuery(term),
    GLEAM_Query(from_term(term))
{
}

void GLEAM_PrfQuery::begin()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(m_handle == 0)
        alloc();

    CGL33::QueryBegin(m_type, m_handle);
#endif
}

void GLEAM_PrfQuery::end()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::QueryEnd(m_type);
#endif
}

int64 GLEAM_PrfQuery::resulti()
{
#if !defined(COFFEE_ONLY_GLES20)
#ifdef COFFEE_GLEAM_DESKTOP
    int64 v;
    CGL33::QueryGetObjecti64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
#else
    return 0;
#endif
}

uint64 GLEAM_PrfQuery::resultu()
{
#if !defined(COFFEE_ONLY_GLES20)
#ifdef COFFEE_GLEAM_DESKTOP
    uint64 v;
    CGL33::QueryGetObjectui64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
#else
    return 0;
#endif
}

#ifndef NDEBUG

static CSize v_size = {1280,720};
static GLEAM_Quad_Drawer m_quad_drawer;

#endif

#ifdef NDEBUG
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b)
{
#else
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b),
      m_size(t.size()),
      m_depth_stencil(PixelFormat::Depth24Stencil8,1),
      m_color(PixelFormat::RGBA8,1)
{
    m_quad_drawer = {};

    if(!GL_DEBUG_MODE)
    {
        m_enabled = false;
        return;
    }

    if(CGL33::Tex_SRGB_Supported())
    {
        m_color.dealloc();
        new (&m_color) GLEAM_Surface2D(PixelFormat::SRGB8A8);
    }

    if(GL_CURR_API == GL_4_3)
        m_enabled = CGL43::Debug::InternalFormatSupport(
                    Texture::T2D,PixelFormat::Depth24Stencil8);
    else
        m_enabled = CGL33::Debug::InternalFormatSupport(
                    Texture::T2D,PixelFormat::Depth24Stencil8);

    if(!m_enabled &&
            (GL_CURR_API == GLES_2_0
             ||GL_CURR_API == GLES_3_0
             ||GL_CURR_API == GLES_3_2))
    {
        m_enabled = true;
        cVerbose(6,"Opting for DEPTH16 depth buffer format");
        m_depth_stencil.dealloc();
        new (&m_depth_stencil) GLEAM_Surface2D(PixelFormat::Depth16,1);
        new (&m_color) GLEAM_Surface2D(PixelFormat::RGB5A1);
    }

    if(GL_DEBUG_MODE && !m_enabled)
        cWarning("Cannot enable debugging, unsupported depth-stencil format (DEPTH24_STENCIL8)");

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.alloc();

        m_color_sampler.alloc();
        m_depth_stencil_sampler.alloc();

        m_color_sampler.attach(&m_color);
        m_depth_stencil_sampler.attach(&m_depth_stencil);
//        m_depth_stencil_sampler.enableShadowSampler();

        resize(m_size);

        m_dtarget.attachSurface(m_color,0,0);
        m_dtarget.attachDepthStencilSurface(m_depth_stencil,0);

        if(!m_dtarget.validate())
        {
            cWarning("Failed to create framebuffer");
            m_enabled = false;

            m_dtarget.dealloc();

            m_color_sampler.dealloc();
            m_depth_stencil_sampler.dealloc();

            return;
        }

        m_quad_drawer.create();
    }
#endif
}

GLEAM_DBufQuery::~GLEAM_DBufQuery()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.dealloc();
        m_quad_drawer.cleanup();
    }
#endif
}

void GLEAM_DBufQuery::resize(const CSize &s)
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        cVerbose(5,"New framebuffer dimensions: {0}",s);
        m_color.allocate(s,PixCmp::RGBA);
        m_depth_stencil.allocate(s,PixCmp::DepthStencil);
    }
#endif
}

void GLEAM_DBufQuery::begin()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
        m_dtarget.bind(FramebufferT::All);
#endif
}

void GLEAM_DBufQuery::end()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        m_rtarget.bind(FramebufferT::All);

        /* Frame data */
        Matf4 transform = scale(Matf4(),Vecf3(0.5));

        /* Draw depth buffer view */
        transform = translation(transform,Vecf3(-1,-1,0));
        m_quad_drawer.draw(transform, m_depth_stencil_sampler);

        /* Draw color buffer view */
        transform = translation(transform,Vecf3(2,0,0));
        m_quad_drawer.draw(transform, m_color_sampler);
    }
#endif
}

GLEAM_ScopeMarker::GLEAM_ScopeMarker(cstring tag)
    :GraphicsDebug::ScopeMarker(tag)
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::Debug::SetDebugGroup(tag, 0);
#endif
}

GLEAM_ScopeMarker::~GLEAM_ScopeMarker()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::Debug::UnsetDebugGroup();
#endif
}

}
}
}
