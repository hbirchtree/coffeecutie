#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_RenderTarget::resize(uint32 i,CRect64 const& view)
{
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
}

void GLEAM_RenderTarget::clear(uint32 i, Vecf4 const& color)
{
    CGL33::ClearBufferfv(true,i,color);
}

void GLEAM_RenderTarget::clear(bigscalar depth)
{
    scalar tmp_dep = depth;
    CGL33::ClearBufferfv(&tmp_dep);
}

void GLEAM_RenderTarget::clear(uint32 i, const Vecf4 &color, bigscalar depth)
{
    clear(i,color);
    clear(depth);
}

void GLEAM_RenderTarget::clear(bigscalar depth, int32 stencil)
{
    CGL33::ClearBufferfi(depth,(int32)stencil);
}

}
}
}
