#pragma once

#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_types_rhi.h>

#define GLM_API "GLEAM_API::"

namespace Coffee::RHI::GLEAM {

extern UqPtr<DataStore> m_store;

struct PboQueue
{
    /* TODO: Convert these to use handle semantics */
    struct Pbo
    {
        u32 buf;
        u32 flags;
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

struct InstanceData
{
    InstanceData() :
        framebufferBinds({{FramebufferT::Draw, 0}, {FramebufferT::Read, 0}})
    {
        GL_CACHED.NUM_PROGRAM_BINARY_FORMATS = -1;
    }

    ~InstanceData()
    {
#if GL_VERSION_VERIFY(0x300, 0x300)
        for(auto const& pbo : pboQueue.buffers)
            gl::core::v33::delete_buffers(SpanOne(pbo.buf));
#endif
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    PboQueue                 pboQueue;
    Vector<ShPtr<GLEAM_TimeQuery>> queries;
#endif

#if GL_VERSION_VERIFY(0x400, 0x310)
    ShPtr<GLEAM_IndirectBuffer> indirectBuf;
#endif

    struct
    {
        i32 NUM_PROGRAM_BINARY_FORMATS;
    } GL_CACHED;

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
} // namespace Coffee::RHI::GLEAM

#define GLEAM_API_THREAD m_store->GpuThread
#define GL_CURR_API m_store->CURR_API
#define GL_DEBUG_MODE m_store->DEBUG_MODE
#define GLEAM_API_INSTANCE_DATA m_store->inst_data
#define GLEAM_FEATURES RHI::GLEAM::m_store->features
#define GLEAM_OPTIONS RHI::GLEAM::m_store->options
#define fb_cached_binds GLEAM_API_INSTANCE_DATA->framebufferBinds

#define GLEAM_VERSION_CHECK(desktop_ver, es_ver)          \
    ((APILevelIsOfClass(GL_CURR_API, APIClass::GLCore) && \
      GL_CURR_API > desktop_ver) ||                       \
     (APILevelIsOfClass(GL_CURR_API, APIClass::GLES) && GL_CURR_API > es_ver))
