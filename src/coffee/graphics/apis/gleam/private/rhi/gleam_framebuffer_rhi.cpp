#include <coffee/graphics/apis/gleam/rhi/gleam_framebuffer_rhi.h>

#include "gleam_internal_types.h"
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>

namespace Coffee {
namespace RHI {
namespace GLEAM {

FORCEDINLINE FramebufferT
             fb_check_binding(FramebufferT t, FramebufferT b, glhnd const& h)
{
    if(feval(t & b) && fb_cached_binds[b] != h.hnd)
    {
        fb_cached_binds[b] = h.hnd;
        return b;
    }
    return FramebufferT::None;
}

FORCEDINLINE void fb_bind(FramebufferT t, glhnd const& h)
{
    FramebufferT b = fb_check_binding(t, FramebufferT::Read, h) |
                     fb_check_binding(t, FramebufferT::Draw, h);
    if(b == FramebufferT::None)
        return;
    gl::v33::FBBind(b, h);
}

void GLEAM_RenderDummy::allocate(PixFmt fmt, DBuffers buf, u32 index, Size size)
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        gl::v45::RBufAllocEx(m_handle.hnd);
    else
#endif
        gl::v33::RBufAlloc(m_handle.hnd);

    gl::vlow::RBufBind(GL_RENDERBUFFER, m_handle);
    gl::vlow::RBufStorage(GL_RENDERBUFFER, fmt, size);
    gl::vlow::RBufBind(GL_RENDERBUFFER, {});

    m_type       = buf;
    m_attachment = index;
}

void GLEAM_RenderDummy::deallocate()
{
    gl::vlow::RBufFree(m_handle.hnd);
    m_handle.release();
}

/* TODO: Cache the currently bound framebuffers for each type to avoid too many
 * rebinds */

void GLEAM_RenderTarget::alloc()
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        gl::v45::FBAllocEx(m_handle.hnd);
    else
#endif
        gl::vlow::FBAlloc(m_handle.hnd);
}

void GLEAM_RenderTarget::dealloc()
{
    gl::vlow::FBFree(m_handle.hnd);
    m_handle.release();
}

void GLEAM_RenderTarget::attachSurface(
    const GLEAM_Surface2D& s, u32 idx, u32 mip)
{
    fb_bind(m_type, m_handle);

    CGenum attachment = GL_COLOR_ATTACHMENT0 + idx;

    gl::vlow::FBTexture2D(
        m_type, attachment, s.m_type, s.m_handle, C_FCAST<i32>(mip));

    if(m_handle.hnd != 0)
        fb_bind(m_type, glhnd());
}

void GLEAM_RenderTarget::attachSurface(const GLEAM_RenderDummy& rb)
{
    fb_bind(m_type, m_handle);

    /* Get GL_*_ATTACHMENT enum */
    CGenum attachment = CGL::to_enum1(rb.m_type);
    if(rb.m_type == DBuffers::Color)
        attachment += rb.m_attachment;

    gl::vlow::FBRenderbuffer(
        m_type, attachment, GL_RENDERBUFFER, rb.m_handle);

    if(m_handle.hnd != 0)
        fb_bind(m_type, glhnd());
}

void GLEAM_RenderTarget::attachDepthStencilSurface(
    const GLEAM_Surface2D& s, u32 mip)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        fb_bind(m_type, m_handle);

        gl::v33::FBTexture2D(
            m_type,
            GL_DEPTH_STENCIL_ATTACHMENT,
            s.m_type,
            s.m_handle,
            C_FCAST<i32>(mip));

        if(m_handle.hnd != 0)
            fb_bind(m_type, glhnd());
    } else
#endif
    {
        attachDepthSurface(s, mip);
    }
}

void GLEAM_RenderTarget::attachDepthSurface(const GLEAM_Surface2D& s, u32 mip)
{
    fb_bind(m_type, m_handle);

    gl::vlow::FBTexture2D(
        m_type, GL_DEPTH_ATTACHMENT, s.m_type, s.m_handle, C_FCAST<i32>(mip));

    if(m_handle.hnd != 0)
        fb_bind(m_type, glhnd());
}

void GLEAM_RenderTarget::blit(
    UNUSED_PARAM(Rect64 const&, src),
    UNUSED_PARAM(GLEAM_RenderTarget&, target),
    UNUSED_PARAM(Rect64 const&, tgt),
    UNUSED_PARAM(DBuffers, buf),
    UNUSED_PARAM(Filtering, flt))
{
    GLEAM_ScopeMarker sc(GLM_API "blit");

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        this->bind(FramebufferT::Read);
        target.bind(FramebufferT::Draw);

        auto srcI = src.convert<i32>();
        auto trgI = src.convert<i32>();
        gl::v33::FBBlit(
            srcI.x,
            srcI.y,
            srcI.x + srcI.w,
            srcI.y + srcI.h,
            trgI.x,
            trgI.y,
            trgI.x + trgI.w,
            trgI.y + trgI.h,
            to_enum2(buf),
            to_enum(flt));

        this->unbind(FramebufferT::Read);
        target.unbind(FramebufferT::Draw);
    }
    // TODO: We could implement this with a slow method and give lots of
    // errors
#endif
}

void GLEAM_RenderTarget::resize(u32 i, Rect64 const& view)
{
    m_size = view.convert<u32>().size();

    gl::v33::FBBind(FramebufferT::Draw, m_handle);
    gl::v33::Viewport({0, 0}, m_size);
    glhnd borrow(fb_cached_binds[FramebufferT::Draw]);
    gl::v33::FBBind(FramebufferT::Draw, borrow);
    borrow.release();
}

Size GLEAM_RenderTarget::size()
{
    Size out;

    if(m_size.area() == 0)
    {
        if(m_handle.hnd != 0)
        {
            fb_bind(FramebufferT::Read, m_handle);
            //            out = gl::v33::FBGetAttachmentSize(FramebufferT::Read,
            //            0);
        } else
        {
            out = CGL::Debug::GetViewport();
        }
        m_size = out;
    } else
        out = m_size;

    return out;
}

void GLEAM_RenderTarget::clear(C_UNUSED(u32 i), Vecf4 const& color)
{
    fb_bind(FramebufferT::Draw, m_handle);
    if(GLEAM_FEATURES.is_gles)
    {
        gl::v33::ClearColor(color);
        gl::v33::Clear(GL_COLOR_BUFFER_BIT);
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    else
        gl::v43::BufClearfv(
            GL_COLOR, C_FCAST<i32>(i), Span<scalar>::Create(color));
#endif
}

void GLEAM_RenderTarget::clear(f64 depth)
{
    fb_bind(FramebufferT::Draw, m_handle);
    scalar tmp_dep = C_CAST<scalar>(depth);
    if(GLEAM_FEATURES.is_gles)
    {
#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
        gl::v33::ClearDepth(depth);
#else
        gl::vlow::ClearDepthf(C_CAST<f32>(depth));
#endif
        gl::v33::Clear(GL_DEPTH_BUFFER_BIT);
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    else
        gl::v33::BufClearfv(GL_DEPTH, 0, tmp_dep);
#endif
}

void GLEAM_RenderTarget::clear(f64 depth, i32 stencil)
{
    fb_bind(FramebufferT::Draw, m_handle);
    if(GLEAM_FEATURES.is_gles)
    {
        //        glClearDepthf(C_CAST<scalar>(depth));
        gl::v33::ClearStencil(stencil);
        gl::v33::Clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    else
        gl::v33::BufClearfi(
            GL_DEPTH_STENCIL, 0, C_CAST<scalar>(depth), C_CAST<i32>(stencil));
#endif
}

void GLEAM_RenderTarget::clear(u32 i, const Vecf4& color, f64 depth)
{
    clear(i, color);
    clear(depth);
}

void GLEAM_RenderTarget::clear(
    u32 i, const Vecf4& color, f64 depth, i32 stencil)
{
    clear(i, color);
    clear(depth, stencil);
}

void GLEAM_RenderTarget::bind(FramebufferT t)
{
    fb_bind(t, m_handle);
    if(m_size.area() == 0)
        size();
    gl::v33::Viewport({0, 0}, m_size);
}

void GLEAM_RenderTarget::unbind(FramebufferT t)
{
    if(m_handle.hnd != 0)
        fb_bind(t, glhnd());
}

bool GLEAM_RenderTarget::validate() const
{
    if(GL_DEBUG_MODE && false)
    {
        fb_bind(m_type, m_handle);
        bool stat = gl::v33::FBCheckStatus(m_type);
        fb_bind(m_type, glhnd());
        return stat;
    } else
        return true;
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
