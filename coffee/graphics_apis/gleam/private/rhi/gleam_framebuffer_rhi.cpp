#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

/* TODO: Cache the currently bound framebuffers for each type to avoid too many rebinds */

void GLEAM_RenderTarget::alloc()
{
    CGL33::FBAlloc(1,&m_handle);
}

void GLEAM_RenderTarget::dealloc()
{
    CGL33::FBFree(1,&m_handle);
}

void GLEAM_RenderTarget::attachSurface(const GLEAM_Surface &, uint32 idx, uint32 mip)
{

}

void GLEAM_RenderTarget::attachSurface(const GLEAM_RenderDummy &rb)
{
    CGL33::FBBind(m_type,m_handle);
    CGL33::FBAttachRenderBuffer(m_type,GL_RENDERBUFFER,0);
    CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::attachDepthStencilSurface(const GLEAM_Surface &s, uint32 idx)
{

}

void GLEAM_RenderTarget::attachDepthSurface(const GLEAM_Surface &s, uint32 idx)
{

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
    CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::clear(uint32 i, Vecf4 const& color)
{
    CGL33::FBBind(m_type,m_handle);
    CGL33::ClearBufferfv(true,i,color);
    CGL33::FBBind(m_type,0);
}

void GLEAM_RenderTarget::clear(bigscalar depth)
{
    CGL33::FBBind(m_type,m_handle);
    scalar tmp_dep = depth;
    CGL33::ClearBufferfv(&tmp_dep);
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
    CGL33::FBBind(t,0);
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
