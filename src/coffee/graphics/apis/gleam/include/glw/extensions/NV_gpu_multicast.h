#pragma once

#ifdef GL_NV_gpu_multicast
#include "../enums/BufferStorageMask.h"
namespace gl::nv::gpu_multicast {
using gl::group::buffer_storage_mask;
namespace values {
constexpr libc_types::u32 multicast_gpus                         = 0x92BA;
constexpr libc_types::u32 per_gpu_storage                        = 0x9548;
constexpr libc_types::u32 multicast_programmable_sample_location = 0x9549;
constexpr libc_types::u32 render_gpu_mask                        = 0x9558;
} // namespace values
STATICINLINE void multicast_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastBarrierNV)
    }
    glMulticastBarrierNV();
    detail::error_check("MulticastBarrierNV"sv);
}

STATICINLINE void multicast_blit_framebuffer(
    u32                      srcGpu,
    u32                      dstGpu,
    i32                      srcX0,
    i32                      srcY0,
    i32                      srcX1,
    i32                      srcY1,
    i32                      dstX0,
    i32                      dstY0,
    i32                      dstX1,
    i32                      dstY1,
    group::clear_buffer_mask mask,
    GLenum                   filter)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastBlitFramebufferNV)
    }
    glMulticastBlitFramebufferNV(
        srcGpu,
        dstGpu,
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        static_cast<GLenum>(mask),
        filter);
    detail::error_check("MulticastBlitFramebufferNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void multicast_buffer_sub_data(
        GLbitfield             gpuMask,
        u32                    buffer,
        GLintptr               offset,
        GLsizeiptr             size,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastBufferSubDataNV)
        glIsBuffer(buffer);
    }
    glMulticastBufferSubDataNV(
        gpuMask,
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("MulticastBufferSubDataNV"sv);
}

STATICINLINE void multicast_copy_buffer_sub_data(
    u32        readGpu,
    GLbitfield writeGpuMask,
    u32        readBuffer,
    u32        writeBuffer,
    GLintptr   readOffset,
    GLintptr   writeOffset,
    GLsizeiptr size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastCopyBufferSubDataNV)
    }
    glMulticastCopyBufferSubDataNV(
        readGpu,
        writeGpuMask,
        readBuffer,
        writeBuffer,
        readOffset,
        writeOffset,
        size);
    detail::error_check("MulticastCopyBufferSubDataNV"sv);
}

STATICINLINE void multicast_copy_image_sub_data(
    u32        srcGpu,
    GLbitfield dstGpuMask,
    u32        srcName,
    GLenum     srcTarget,
    i32        srcLevel,
    i32        srcX,
    i32        srcY,
    i32        srcZ,
    u32        dstName,
    GLenum     dstTarget,
    i32        dstLevel,
    i32        dstX,
    i32        dstY,
    i32        dstZ,
    i32        srcWidth,
    i32        srcHeight,
    i32        srcDepth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastCopyImageSubDataNV)
    }
    glMulticastCopyImageSubDataNV(
        srcGpu,
        dstGpuMask,
        srcName,
        srcTarget,
        srcLevel,
        srcX,
        srcY,
        srcZ,
        dstName,
        dstTarget,
        dstLevel,
        dstX,
        dstY,
        dstZ,
        srcWidth,
        srcHeight,
        srcDepth);
    detail::error_check("MulticastCopyImageSubDataNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void multicast_framebuffer_sample_locationsfv(
        u32 gpu, u32 framebuffer, u32 start, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastFramebufferSampleLocationsfvNV)
        glIsFramebuffer(framebuffer);
    }
    glMulticastFramebufferSampleLocationsfvNV(
        gpu,
        framebuffer,
        start,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("MulticastFramebufferSampleLocationsfvNV"sv);
}

template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void multicast_get_query_objecti64v(
        u32 gpu, u32 id, GLenum pname, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastGetQueryObjecti64vNV)
    }
    glMulticastGetQueryObjecti64vNV(
        gpu,
        id,
        pname,
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("MulticastGetQueryObjecti64vNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multicast_get_query_objectiv(
        u32 gpu, u32 id, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastGetQueryObjectivNV)
    }
    glMulticastGetQueryObjectivNV(
        gpu,
        id,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("MulticastGetQueryObjectivNV"sv);
}

template<class span_u64>
requires(semantic::concepts::Span<span_u64>&& std::is_same_v<
         std::decay_t<typename span_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void multicast_get_query_objectui64v(
        u32 gpu, u32 id, GLenum pname, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastGetQueryObjectui64vNV)
    }
    glMulticastGetQueryObjectui64vNV(
        gpu,
        id,
        pname,
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("MulticastGetQueryObjectui64vNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void multicast_get_query_objectuiv(
        u32 gpu, u32 id, GLenum pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastGetQueryObjectuivNV)
    }
    glMulticastGetQueryObjectuivNV(
        gpu,
        id,
        pname,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("MulticastGetQueryObjectuivNV"sv);
}

STATICINLINE void multicast_wait_sync(u32 signalGpu, GLbitfield waitGpuMask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastWaitSyncNV)
    }
    glMulticastWaitSyncNV(signalGpu, waitGpuMask);
    detail::error_check("MulticastWaitSyncNV"sv);
}

STATICINLINE void render_gpu_mask(GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderGpuMaskNV)
    }
    glRenderGpuMaskNV(mask);
    detail::error_check("RenderGpuMaskNV"sv);
}

} // namespace gl::nv::gpu_multicast
#endif // GL_NV_gpu_multicast
namespace gl::nv::gpu_multicast {
constexpr auto name = "GL_NV_gpu_multicast";
}
