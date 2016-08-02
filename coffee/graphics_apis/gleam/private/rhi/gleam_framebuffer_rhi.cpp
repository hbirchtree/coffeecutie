#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

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
    CGL33::FBBind(m_type,m_handle);

    CGenum attachment = GL_COLOR_ATTACHMENT0 + idx;

    CGL33::FBAttachTexture2D(m_type,attachment,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::attachSurface(const GLEAM_RenderDummy &rb)
{
    CGL33::FBBind(m_type,m_handle);

    /* Get GL_*_ATTACHMENT enum */
    CGenum attachment = CGL::to_enum1(rb.m_type);
    if(rb.m_type == DBuffers::Color)
        attachment += rb.m_attachment;

    CGL33::FBAttachRenderBuffer(m_type,attachment,rb.m_handle);

    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::attachDepthStencilSurface(const GLEAM_Surface &s, uint32 mip)
{
    CGL33::FBBind(m_type,m_handle);

    CGL33::FBAttachTexture2D(m_type,GL_DEPTH_STENCIL_ATTACHMENT,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::attachDepthSurface(const GLEAM_Surface &s, uint32 mip)
{
    CGL33::FBBind(m_type,m_handle);

    CGL33::FBAttachTexture2D(m_type,GL_DEPTH_ATTACHMENT,s.m_type,s.m_handle,mip);

    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::blit(const CRect64 &src, GLEAM_RenderTarget &target,
                              const CRect64 &tgt, DBuffers buf,
                              Filtering flt) const
{
    this->bind(FramebufferT::Read);
    target.bind(FramebufferT::Draw);
    CGL33::FBBlit(src.convert<int32>(),tgt.convert<int32>(),buf,flt);

    this->unbind(FramebufferT::Read);
    target.unbind(FramebufferT::Draw);
}

void GLEAM_RenderTarget::resize(uint32 i,CRect64 const& view)
{
    CGL33::FBBind(m_type,m_handle);
    if(CGL43::ViewportArraySupported())
    {
        CRectF view_f = {(scalar)view.x,(scalar)view.y,
                         (scalar)view.w,(scalar)view.h};
        CGL43::ViewportSet(i,&view_f);
    }else{
        if(GL_DEBUG_MODE)
            if(i != 0)
                cWarning("Cannot perform task: applying viewport index, unsupported");
        if(i==0)
            CGL33::ViewportSet(&view);
    }
    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::clear(uint32 i, Vecf4 const& color)
{
    CGL33::FBBind(m_type,m_handle);
    scalar* d = (scalar*)&color;
    CGL33::ClearBufferfv(true,i,color);
    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::clear(bigscalar depth)
{
    CGL33::FBBind(m_type,m_handle);
    scalar tmp_dep = depth;
    CGL33::ClearBufferfv(&tmp_dep);
    if(m_handle != 0)
        CGL33::FBBind(m_type,0);
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
    CGL33::FBBind(t,m_handle);
}

void GLEAM_RenderTarget::unbind(FramebufferT t) const
{
    if(m_handle != 0)
        CGL33::FBBind(t,0);
}

bool GLEAM_RenderTarget::validate() const
{
    if(GL_DEBUG_MODE)
    {
        CGL33::FBBind(m_type,m_handle);
        bool stat = CGL33::FBValidate(m_type);
        CGL33::FBBind(m_type,0);
        return stat;
    }else
        return true;
}

void GLEAM_RenderTarget::clear(bigscalar depth, int32 stencil)
{
    CGL33::FBBind(m_type,m_handle);
    CGL33::ClearBufferfi(depth,(int32)stencil);
    CGL33::FBBind(m_type,0);
}

}
}
}
