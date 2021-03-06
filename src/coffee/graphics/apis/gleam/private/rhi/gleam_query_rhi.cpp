#include "gleam_internal_types.h"
#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>

namespace Coffee {
namespace RHI {
namespace GLEAM {

void GLEAM_Query::alloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        CGL45::QueryAllocEx(m_type, m_handle);
    else
#endif
        if(!GLEAM_FEATURES.gles20)
        CGL33::QueryAlloc(m_handle);
#endif
}

void GLEAM_Query::dealloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        CGL33::QueryFree(m_handle);
#endif
}

void GLEAM_OccludeQuery::begin()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        if(m_handle == 0)
            alloc();
        CGL33::QueryBegin(m_type, m_handle);
        CGL33::ColorMask({0, 0, 0, 0});
        CGL33::DepthMask(false);
    }
#endif
}

void GLEAM_OccludeQuery::end()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::ColorMask({1, 1, 1, 1});
        CGL33::DepthMask(true);
        CGL33::QueryEnd(m_type);
    }
#endif
}

i64 GLEAM_OccludeQuery::resulti()
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

u64 GLEAM_OccludeQuery::resultu()
{
#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
    uint64 v;
    CGL33::QueryGetObjectui64v(m_handle, GL_QUERY_RESULT, &v);
#elif GL_VERSION_VERIFY(0x200, 0x300)
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle, GL_QUERY_RESULT, &v);
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
    return v;
#else
    return 0;
#endif
}

void GLEAM_TimeQuery::begin()
{
#if GL_VERSION_VERIFY(0x200, 0x300)
    CGL33::QueryBegin(m_type, m_handle);
#endif
}

void GLEAM_TimeQuery::end()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    CGL33::QueryEnd(m_type);
#endif
}

bool GLEAM_TimeQuery::available()
{
#if GL_VERSION_VERIFY(0x200, 0x300)
    u32 v;
    CGL33::QueryGetObjectuiv(m_handle, GL_QUERY_RESULT_AVAILABLE, &v);

    return v == GL_TRUE;
#else
    return true;
#endif
}

u64 GLEAM_TimeQuery::result()
{
#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
    u64 v;
    CGL33::QueryGetObjectui64v(m_handle, GL_QUERY_RESULT, &v);
    return v;
#elif GL_VERSION_VERIFY(0x200, 0x300)
    u32 v;
    CGL33::QueryGetObjectuiv(m_handle, GL_QUERY_RESULT, &v);
    return v;
#else
    return 0;
#endif
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
