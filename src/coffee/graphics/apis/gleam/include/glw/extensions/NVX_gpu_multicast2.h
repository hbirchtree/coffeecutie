#pragma once

#ifdef GL_NVX_gpu_multicast2
namespace gl::nvx::gpu_multicast2 {
namespace values {
constexpr libc_types::u32 upload_gpu_mask = 0x954A;
} // namespace values
template<class span_const_u32, class span_const_u64>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_const_u64>&&
                              std::is_same_v<
                 std::decay_t<typename span_const_u64::value_type>,
                 std::decay_t<u64>>) STATICINLINE GLuint
    async_copy_buffer_sub_data(
        span_const_u32 const& waitSemaphoreArray,
        span_const_u64 const& fenceValueArray,
        u32                   readGpu,
        GLbitfield            writeGpuMask,
        u32                   readBuffer,
        u32                   writeBuffer,
        GLintptr              readOffset,
        GLintptr              writeOffset,
        GLsizeiptr            size,
        span_const_u32 const& signalSemaphoreArray,
        span_const_u64 const& signalValueArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AsyncCopyBufferSubDataNVX)
    }
    auto out = glAsyncCopyBufferSubDataNVX(
        waitSemaphoreArray.size(),
        waitSemaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(waitSemaphoreArray.data())
            : nullptr,
        fenceValueArray.size()
            ? reinterpret_cast<const GLuint64*>(fenceValueArray.data())
            : nullptr,
        readGpu,
        writeGpuMask,
        readBuffer,
        writeBuffer,
        readOffset,
        writeOffset,
        size,
        signalSemaphoreArray.size(),
        signalSemaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(signalSemaphoreArray.data())
            : nullptr,
        signalValueArray.size()
            ? reinterpret_cast<const GLuint64*>(signalValueArray.data())
            : nullptr);
    detail::error_check("AsyncCopyBufferSubDataNVX"sv);
    return out;
}

template<class span_const_u32, class span_const_u64>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_const_u64>&&
                              std::is_same_v<
                 std::decay_t<typename span_const_u64::value_type>,
                 std::decay_t<u64>>) STATICINLINE GLuint
    async_copy_image_sub_data(
        span_const_u32 const& waitSemaphoreArray,
        span_const_u64 const& waitValueArray,
        u32                   srcGpu,
        GLbitfield            dstGpuMask,
        u32                   srcName,
        GLenum                srcTarget,
        i32                   srcLevel,
        i32                   srcX,
        i32                   srcY,
        i32                   srcZ,
        u32                   dstName,
        GLenum                dstTarget,
        i32                   dstLevel,
        i32                   dstX,
        i32                   dstY,
        i32                   dstZ,
        i32                   srcWidth,
        i32                   srcHeight,
        i32                   srcDepth,
        span_const_u32 const& signalSemaphoreArray,
        span_const_u64 const& signalValueArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AsyncCopyImageSubDataNVX)
    }
    auto out = glAsyncCopyImageSubDataNVX(
        waitSemaphoreArray.size(),
        waitSemaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(waitSemaphoreArray.data())
            : nullptr,
        waitValueArray.size()
            ? reinterpret_cast<const GLuint64*>(waitValueArray.data())
            : nullptr,
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
        srcDepth,
        signalSemaphoreArray.size(),
        signalSemaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(signalSemaphoreArray.data())
            : nullptr,
        signalValueArray.size()
            ? reinterpret_cast<const GLuint64*>(signalValueArray.data())
            : nullptr);
    detail::error_check("AsyncCopyImageSubDataNVX"sv);
    return out;
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multicast_scissor_arrayv(
        u32 gpu, u32 first, i32 count, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastScissorArrayvNVX)
    }
    glMulticastScissorArrayvNVX(
        gpu,
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("MulticastScissorArrayvNVX"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void multicast_viewport_arrayv(
        u32 gpu, u32 first, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastViewportArrayvNVX)
    }
    glMulticastViewportArrayvNVX(
        gpu,
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("MulticastViewportArrayvNVX"sv);
}

STATICINLINE void multicast_viewport_position_w_scale(
    u32 gpu, u32 index, f32 xcoeff, f32 ycoeff)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MulticastViewportPositionWScaleNVX)
    }
    glMulticastViewportPositionWScaleNVX(gpu, index, xcoeff, ycoeff);
    detail::error_check("MulticastViewportPositionWScaleNVX"sv);
}

STATICINLINE void upload_gpu_mask(GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UploadGpuMaskNVX)
    }
    glUploadGpuMaskNVX(mask);
    detail::error_check("UploadGpuMaskNVX"sv);
}

} // namespace gl::nvx::gpu_multicast2
#endif // GL_NVX_gpu_multicast2
namespace gl::nvx::gpu_multicast2 {
constexpr auto name = "GL_NVX_gpu_multicast2";
}