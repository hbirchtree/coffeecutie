#include <coffee/graphics_apis/gleam/rhi/gleam_profile_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b)
{
    if(GL_DEBUG_MODE)
        m_debug_target.alloc();
}

GLEAM_DBufQuery::~GLEAM_DBufQuery()
{
    if(GL_DEBUG_MODE)
        m_debug_target.dealloc();
}

void GLEAM_DBufQuery::begin()
{
    if(GL_DEBUG_MODE)
    {
        m_debug_target.bind(FramebufferT::Draw);
    }
}

void GLEAM_DBufQuery::end()
{
    if(GL_DEBUG_MODE)
    {
        Filtering flt = Filtering::None;
        m_debug_target.blit({0,0,512,512},m_rtarget,{0,0,512,512},m_buffers,flt);
    }
}

}
}
}
