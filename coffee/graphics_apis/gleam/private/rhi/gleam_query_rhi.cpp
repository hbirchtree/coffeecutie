#include <coffee/graphics_apis/gleam/rhi/gleam_query_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_Query::alloc()
{
    CGL33::QueryAlloc(1,&m_handle);
}

void GLEAM_Query::dealloc()
{
    if(GL_CURR_API != GL_4_5)
        CGL33::QueryFree(1,&m_handle);
}

void GLEAM_OccludeQuery::begin()
{
    if(m_handle == 0)
        alloc();
    CGL33::QueryBegin(m_type,m_handle);
    CGL33::ColorMask({0,0,0,0});
    CGL33::DepthMask(false);
}

void GLEAM_OccludeQuery::end()
{
    CGL33::ColorMask({1,1,1,1});
    CGL33::DepthMask(true);
    CGL33::QueryEnd(m_type);
}

int64 GLEAM_OccludeQuery::getResulti()
{
#ifdef COFFEE_GLEAM_DESKTOP
    int64 v;
    CGL33::QueryGetObjecti64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
}

uint64 GLEAM_OccludeQuery::getResultu()
{
#ifdef COFFEE_GLEAM_DESKTOP
    uint64 v;
    CGL33::QueryGetObjectui64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
}

}
}
}
