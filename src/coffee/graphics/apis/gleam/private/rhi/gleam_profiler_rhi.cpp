#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>

#include "gleam_internal_types.h"
#include <coffee/graphics/apis/gleam/rhi/gleam_framebuffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_quad_draw.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/strings/info.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace RHI {
namespace GLEAM {

query::flags from_term(ProfilingTerm t)
{
    switch(t)
    {
    case ProfilingTerm::ElapsedTime:
        return query::timing::value;
    case ProfilingTerm::ScreenPrimitives:
        return query::primitives::value;
    case ProfilingTerm::ScreenSamples:
        return query::samples::value;
    case ProfilingTerm::TransformedPrimitives:
        return query::feedback_primitives::value;
    default:
        return query::any_samples::value;
    }
}

GLEAM_PrfQuery::GLEAM_PrfQuery(ProfilingTerm term) :
    PerfQuery(term), GLEAM_Query(from_term(term))
{
}

void GLEAM_PrfQuery::begin()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(m_handle == 0)
        alloc();

    CGL33::QueryBegin(m_type, m_handle);
#endif
}

void GLEAM_PrfQuery::end()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    CGL33::QueryEnd(m_type);
#endif
}

i64 GLEAM_PrfQuery::resulti()
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    int64 v;
    CGL33::QueryGetObjecti64v(m_handle, GL_QUERY_RESULT, &v);
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle, GL_QUERY_RESULT, &v);
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
    return v;
#else
    return 0;
#endif
}

u64 GLEAM_PrfQuery::resultu()
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    uint64 v;
    CGL33::QueryGetObjectui64v(m_handle, GL_QUERY_RESULT, &v);
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle, GL_QUERY_RESULT, &v);
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
    return v;
#else
    return 0;
#endif
}

#if MODE_RELEASE
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t, DBuffers b) :
    GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t, b)
{
#else
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t, DBuffers b) :
    GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t, b), m_size(t.size()),
    m_depth_stencil(PixFmt::Depth24Stencil8, 1), m_color(PixFmt::RGBA8, 1)
{
    auto& m_quad_drawer = *m_store->debug_drawer;

    if(!GL_DEBUG_MODE)
    {
        m_enabled = false;
        return;
    }

    //    if(CGL33::Tex_SRGB_Supported())
    //    {
    //        m_color.dealloc();
    //        new (&m_color) GLEAM_Surface2D(PixFmt::SRGB8A8);
    //    }

    if(GL_CURR_API == GL_4_3)
        m_enabled = CGL::Debug::InternalFormatSupport(PixFmt::Depth24Stencil8);
    else
        m_enabled = CGL::Debug::InternalFormatSupport(PixFmt::Depth24Stencil8);

    if(!m_enabled && (GL_CURR_API == GLES_2_0 || GL_CURR_API == GLES_3_0 ||
                      GL_CURR_API == GLES_3_2))
    {
        m_enabled = true;
        cVerbose(6, "Opting for DEPTH16 depth buffer format");
        m_depth_stencil.dealloc();
        new(&m_depth_stencil) GLEAM_Surface2D(PixFmt::Depth16, 1);
        new(&m_color) GLEAM_Surface2D(PixFmt::RGB5A1);
    }

    if(GL_DEBUG_MODE && !m_enabled)
        cWarning("Cannot enable debugging, unsupported depth-stencil format "
                 "(DEPTH24_STENCIL8)");

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.alloc();

        m_color_sampler.alloc();
        m_depth_stencil_sampler.alloc();

        m_color_sampler.attach(&m_color);
        m_depth_stencil_sampler.attach(&m_depth_stencil);
        //        m_depth_stencil_sampler.enableShadowSampler();

        resize(m_size);

        m_dtarget.attachSurface(m_color, 0, 0);
        m_dtarget.attachDepthStencilSurface(m_depth_stencil, 0);

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
#if MODE_DEBUG
    auto& m_quad_drawer = *m_store->debug_drawer;

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.dealloc();
        m_quad_drawer.cleanup();
    }
#endif
}

void GLEAM_DBufQuery::resize(const Size& s)
{
#if MODE_DEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        cVerbose(5, "New framebuffer dimensions: {0}", s);
        m_color.allocate(s, PixCmp::RGBA);
        m_depth_stencil.allocate(s, PixCmp::DepthStencil);
    }
#endif
}

void GLEAM_DBufQuery::begin()
{
#if MODE_DEBUG
    if(GL_DEBUG_MODE && m_enabled)
        m_dtarget.bind(FramebufferT::All);
#endif
}

void GLEAM_DBufQuery::end()
{
#if MODE_DEBUG
    auto& m_quad_drawer = *m_store->debug_drawer;

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_rtarget.bind(FramebufferT::All);

        /* Frame data */
        Matf4 transform = scale(Matf4(), Vecf3(0.5));

        /* Draw depth buffer view */
        transform = translation(transform, Vecf3(-1, -1, 0));
        m_quad_drawer.draw(transform, m_depth_stencil_sampler);

        /* Draw color buffer view */
        transform = translation(transform, Vecf3(2, 0, 0));
        m_quad_drawer.draw(transform, m_color_sampler);
    }
#endif
}

GLEAM_ScopeMarker::GLEAM_ScopeMarker(cstring tag) :
    GraphicsDebug::ScopeMarker(tag)
{
#if GL_VERSION_VERIFY(0x300, 0x320)
    if(GLEAM_VERSION_CHECK(GL_4_3, GLES_3_2) ||
       Debug::CheckExtensionSupported(CGL_DBG_CTXT, "GL_KHR_debug"))
        CGL43::PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, strlen(tag), tag);
//    else
//        CGL_KHR_debug<int>::PushDebugGroupKHR(
//            GL_DEBUG_SOURCE_APPLICATION_KHR, 0, strlen(tag), tag);
#endif
}

GLEAM_ScopeMarker::~GLEAM_ScopeMarker()
{
#if GL_VERSION_VERIFY(0x300, 0x320)
    if(GLEAM_VERSION_CHECK(GL_4_3, GLES_3_2) ||
       Debug::CheckExtensionSupported(CGL_DBG_CTXT, "GL_KHR_debug"))
        CGL43::PopDebugGroup();
//    else
//        CGL_KHR_debug<int>::PopDebugGroupKHR();
#endif
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
