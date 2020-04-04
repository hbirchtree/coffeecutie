#pragma once

#include <coffee/graphics/apis/gleam/gleam.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_types_rhi.h>

#define GLM_API "GLEAM_API::"

namespace Coffee {
namespace RHI {
namespace GLEAM {

#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
using CGL33 = CGL::CGLES30;
using CGL43 = CGL::CGLES32;
using CGL45 = CGL::CGLES32;
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
using CGL33 = CGL::CGLES20;
using CGL43 = CGL::CGLES20;
#endif

using namespace CGL;

extern GLEAM_DataStore* m_store;

struct GLEAM_PboQueue
{
    /* TODO: Convert these to use handle semantics */
    struct Pbo
    {
        CGhnd buf;
        u32   flags;
    };
    Vector<Pbo> buffers;
    u32         idx = 0;
    Pbo&        current()
    {
        Pbo& ref = buffers.at(idx++);
        idx      = idx % buffers.size();
        return ref;
    }
};

struct GLEAM_Instance_Data
{
    GLEAM_Instance_Data() :
        framebufferBinds({{FramebufferT::Draw, 0}, {FramebufferT::Read, 0}})
    {
        GL_CACHED.NUM_PROGRAM_BINARY_FORMATS = -1;
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    GLEAM_PboQueue                 pboQueue;
    Vector<ShPtr<GLEAM_TimeQuery>> queries;
#endif

#if GL_VERSION_VERIFY(0x400, 0x310)
    ShPtr<GLEAM_IndirectBuffer> indirectBuf;
#endif

    struct
    {
        i32 NUM_PROGRAM_BINARY_FORMATS;
    } GL_CACHED;

    Debug::CGL_Shared_Debug::Context dbgContext;

    Map<FramebufferT, glhnd::handle_type> framebufferBinds;
};

inline APILevel gl_level_from_string(CString const& str)
{
    if(str == "GL33")
        return GL_3_3;
    if(str == "GL43")
        return GL_4_3;
    if(str == "GL45")
        return GL_4_5;
    if(str == "GL46")
        return GL_4_6;
    if(str == "ES2")
        return GLES_2_0;
    if(str == "ES3")
        return GLES_3_0;
    if(str == "ES32")
        return GLES_3_2;
    else
        return GL_Nothing;
}
} // namespace GLEAM
} // namespace RHI
} // namespace Coffee

#define GLEAM_API_THREAD m_store->GpuThread
#define GL_CURR_API m_store->CURR_API
#define GL_DEBUG_MODE m_store->DEBUG_MODE
#define GLEAM_API_INSTANCE_DATA m_store->inst_data
#define GLEAM_FEATURES RHI::GLEAM::m_store->features
#define GLEAM_OPTIONS RHI::GLEAM::m_store->options
#define CGL_DBG_CTXT GLEAM_API_INSTANCE_DATA->dbgContext
#define fb_cached_binds GLEAM_API_INSTANCE_DATA->framebufferBinds

#define GLEAM_VERSION_CHECK(desktop_ver, es_ver)          \
    ((APILevelIsOfClass(GL_CURR_API, APIClass::GLCore) && \
      GL_CURR_API > desktop_ver) ||                       \
     (APILevelIsOfClass(GL_CURR_API, APIClass::GLES) && GL_CURR_API > es_ver))
