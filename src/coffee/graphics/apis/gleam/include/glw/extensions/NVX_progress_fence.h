#pragma once

#ifdef GL_NVX_progress_fence
namespace gl::nvx::progress_fence {
namespace values {
} // namespace values
template<class span_const_u32, class span_const_u64>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    semantic::concepts::Span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_NVX_progress_fence
 * \param fenceObjectCount GLsizei
 * \param semaphoreArray const GLuint *
 * \param fenceValueArray const GLuint64 *
 * \return void
 */
STATICINLINE void client_wait_semaphoreui64(
    span_const_u32 const& semaphoreArray, span_const_u64 const& fenceValueArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientWaitSemaphoreui64NVX)
    }
    glClientWaitSemaphoreui64NVX(
        semaphoreArray.size(),
        semaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(semaphoreArray.data())
            : nullptr,
        fenceValueArray.size()
            ? reinterpret_cast<const GLuint64*>(fenceValueArray.data())
            : nullptr);
    detail::error_check("ClientWaitSemaphoreui64NVX"sv);
}

/*!
 * \brief Part of GL_NVX_progress_fence

 * \return GLuint
 */
STATICINLINE GLuint create_progress_fence()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgressFenceNVX)
    }
    auto out = glCreateProgressFenceNVX();
    detail::error_check("CreateProgressFenceNVX"sv);
    return out;
}

template<class span_const_u32, class span_const_u64>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    semantic::concepts::Span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_NVX_progress_fence
 * \param signalGpu GLuint
 * \param fenceObjectCount GLsizei
 * \param semaphoreArray const GLuint *
 * \param fenceValueArray const GLuint64 *
 * \return void
 */
STATICINLINE void signal_semaphoreui64(
    u32                   signalGpu,
    span_const_u32 const& semaphoreArray,
    span_const_u64 const& fenceValueArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SignalSemaphoreui64NVX)
    }
    glSignalSemaphoreui64NVX(
        signalGpu,
        semaphoreArray.size(),
        semaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(semaphoreArray.data())
            : nullptr,
        fenceValueArray.size()
            ? reinterpret_cast<const GLuint64*>(fenceValueArray.data())
            : nullptr);
    detail::error_check("SignalSemaphoreui64NVX"sv);
}

template<class span_const_u32, class span_const_u64>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    semantic::concepts::Span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_NVX_progress_fence
 * \param waitGpu GLuint
 * \param fenceObjectCount GLsizei
 * \param semaphoreArray const GLuint *
 * \param fenceValueArray const GLuint64 *
 * \return void
 */
STATICINLINE void wait_semaphoreui64(
    u32                   waitGpu,
    span_const_u32 const& semaphoreArray,
    span_const_u64 const& fenceValueArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSemaphoreui64NVX)
    }
    glWaitSemaphoreui64NVX(
        waitGpu,
        semaphoreArray.size(),
        semaphoreArray.size()
            ? reinterpret_cast<const GLuint*>(semaphoreArray.data())
            : nullptr,
        fenceValueArray.size()
            ? reinterpret_cast<const GLuint64*>(fenceValueArray.data())
            : nullptr);
    detail::error_check("WaitSemaphoreui64NVX"sv);
}

} // namespace gl::nvx::progress_fence
#endif // GL_NVX_progress_fence
namespace gl::nvx::progress_fence {
constexpr auto name = "GL_NVX_progress_fence";
}
