#include <coffee/graphics/apis/gleam/rhi/gleam_framebuffer_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

static Map<FramebufferT,CGhnd> fb_cached_binds =
{
    {FramebufferT::Draw,0},
    {FramebufferT::Read,0},
};

FORCEDINLINE FramebufferT fb_check_binding(FramebufferT t,FramebufferT b, CGhnd h)
{
    if(feval(t&b)&&fb_cached_binds[b] != h)
    {
        fb_cached_binds[b] = h;
        return b;
    }
    return FramebufferT::None;
}

FORCEDINLINE void fb_bind(FramebufferT t, CGhnd h)
{
    FramebufferT b =
            fb_check_binding(t,FramebufferT::Read,h)
            |fb_check_binding(t,FramebufferT::Draw,h);
    if(b == FramebufferT::None)
        return;
    CGL33::FBBind(b,h);
}

void GLEAM_RenderDummy::allocate(PixelFormat fmt,DBuffers buf, uint32 index, CSize size)
{
    CGL33::RenderBufferAlloc(1,&m_handle);

    CGL33::RBufBind(m_handle);
    CGL33::RBufStorage(fmt,size.w,size.h);
    CGL33::RBufBind(0);

    m_type = buf;
    m_attachment = index;
}

void GLEAM_RenderDummy::deallocate()
{
    CGL33::RenderBufferFree(1,&m_handle);
}

/* TODO: Cache the currently bound framebuffers for each type to avoid too many rebinds */

void GLEAM_RenderTarget::alloc()
{
    CGL33::FBAlloc(1,&m_handle);
}

void GLEAM_RenderTarget::dealloc()
{
    CGL33::FBFree(1,&m_handle);
}

void GLEAM_RenderTarget::attachSurface(const GLEAM_Surface &s, uint32 idx, uint32 mip)
{
    fb_bind(m_type,m_handle);

    CGenum attachment = GL_COLOR_ATTACHMENT0 + idx;

    CGL33::FBAttachTexture2D(m_type,attachment,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        fb_bind(m_type,0);
}

void GLEAM_RenderTarget::attachSurface(const GLEAM_RenderDummy &rb)
{
    fb_bind(m_type,m_handle);

    /* Get GL_*_ATTACHMENT enum */
    CGenum attachment = CGL::to_enum1(rb.m_type);
    if(rb.m_type == DBuffers::Color)
        attachment += rb.m_attachment;

    CGL33::FBAttachRenderBuffer(m_type,attachment,rb.m_handle);

    if(m_handle != 0)
        fb_bind(m_type,0);
}

void GLEAM_RenderTarget::attachDepthStencilSurface(const GLEAM_Surface &s, uint32 mip)
{
#if !defined(COFFEE_ONLY_GLES20)
    fb_bind(m_type,m_handle);

    CGL33::FBAttachTexture2D(m_type,GL_DEPTH_STENCIL_ATTACHMENT,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        fb_bind(m_type,0);
#else
    attachDepthSurface(s, mip);
#endif
}

void GLEAM_RenderTarget::attachDepthSurface(const GLEAM_Surface &s, uint32 mip)
{
    fb_bind(m_type,m_handle);

    CGL33::FBAttachTexture2D(m_type,GL_DEPTH_ATTACHMENT,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        fb_bind(m_type,0);
}

void GLEAM_RenderTarget::blit(const CRect64 &src, GLEAM_RenderTarget &target,
                              const CRect64 &tgt, DBuffers buf,
                              Filtering flt) const
{
    C_USED(src);
    C_USED(target);
    C_USED(tgt);
    C_USED(buf);
    C_USED(flt);
#if !defined(COFFEE_ONLY_GLES20)
    this->bind(FramebufferT::Read);
    target.bind(FramebufferT::Draw);
    CGL33::FBBlit(src.convert<int32>(),tgt.convert<int32>(),buf,flt);

    this->unbind(FramebufferT::Read);
    target.unbind(FramebufferT::Draw);
#else
    //TODO: We could implement this with a slow method and give lots of errors
#endif
}

void GLEAM_RenderTarget::resize(uint32 i,CRect64 const& view)
{
    fb_bind(m_type,m_handle);
    auto sz_arm_printable = view.convert<i32>();
    cVerbose(10, "Resizing render target {0} to {1}x{2}", m_handle,
             sz_arm_printable.w, sz_arm_printable.h);

    m_size = sz_arm_printable.size();

#if !defined(COFFEE_ONLY_GLES20)
    if(CGL43::ViewportArraySupported())
    {
        auto r = view.convert<scalar>();
        CGL43::ViewportSet(i,r);
    }else
#endif
    {
        if(GL_DEBUG_MODE)
            if(i != 0)
                cWarning("Cannot perform task: applying viewport index, unsupported");
        if(i==0)
            CGL33::ViewportSet(view);
    }
    if(m_handle != 0)
        fb_bind(m_type,0);
}

CSize GLEAM_RenderTarget::size()
{
    CSize out;

    if(m_size.area() == 0)
        if(m_handle != 0)
        {
            fb_bind(m_type,m_handle);

            out = CGL33::FBGetAttachmentSize(m_type,0);

            if(m_handle != 0)
                fb_bind(m_type,m_handle);
        }else{
            out = CGL33::Debug::GetViewport();
        }
    else
        out = m_size;

    return out;
}

void GLEAM_RenderTarget::clear(uint32 i, Vecf4 const& color)
{
    C_USED(i);
    fb_bind(m_type,m_handle);
//    scalar* d = (scalar*)&color;
//    if(GL_CURR_API == GLES_3_0)
//    {
        glClearColor(color.r(),color.g(),color.b(),color.a());
        glClear(GL_COLOR_BUFFER_BIT);
//    }else
//        CGL33::ClearBufferfv(true,i,color);
}

void GLEAM_RenderTarget::clear(bigscalar depth)
{
    fb_bind(m_type,m_handle);
    scalar tmp_dep = C_CAST<scalar>(depth);
    if(GL_CURR_API == GLES_3_0)
    {
        glClearDepthf(C_CAST<scalar>(depth));
        glClear(GL_DEPTH_BUFFER_BIT);
    }else
        CGL33::ClearBufferfv(&tmp_dep);
}

void GLEAM_RenderTarget::clear(bigscalar depth, int32 stencil)
{
    fb_bind(m_type,m_handle);
    if(GL_CURR_API == GLES_3_0)
    {
//        glClearDepthf(C_CAST<scalar>(depth));
        glClearStencil(stencil);
        glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    }else
        CGL33::ClearBufferfi(C_CAST<scalar>(depth),
                             C_CAST<int32>(stencil));
}

void GLEAM_RenderTarget::clear(uint32 i, const Vecf4 &color, bigscalar depth)
{
    clear(i,color);
    clear(depth);
}

void GLEAM_RenderTarget::clear(uint32 i, const Vecf4 &color, bigscalar depth, int32 stencil)
{
    clear(i,color);
    clear(depth,stencil);
}

void GLEAM_RenderTarget::bind(FramebufferT t) const
{
    fb_bind(t,m_handle);
}

void GLEAM_RenderTarget::unbind(FramebufferT t) const
{
    if(m_handle != 0)
        fb_bind(t,0);
}

bool GLEAM_RenderTarget::validate() const
{
    if(GL_DEBUG_MODE)
    {
        fb_bind(m_type,m_handle);
        bool stat = CGL33::FBValidate(m_type);
        fb_bind(m_type,0);
        return stat;
    }else
        return true;
}

}
}
}
