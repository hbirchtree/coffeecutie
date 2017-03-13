#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_Query::alloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::QueryAlloc(1,&m_handle);
#endif
}

void GLEAM_Query::dealloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API != GL_4_5)
        CGL33::QueryFree(1,&m_handle);
#endif
}

void GLEAM_OccludeQuery::begin()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(m_handle == 0)
        alloc();
    CGL33::QueryBegin(m_type,m_handle);
    CGL33::ColorMask({0,0,0,0});
    CGL33::DepthMask(false);
#endif
}

void GLEAM_OccludeQuery::end()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::ColorMask({1,1,1,1});
    CGL33::DepthMask(true);
    CGL33::QueryEnd(m_type);
#endif
}

int64 GLEAM_OccludeQuery::resulti()
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

uint64 GLEAM_OccludeQuery::resultu()
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

}
}
}
