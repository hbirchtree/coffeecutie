#pragma once

#ifdef GL_AMD_framebuffer_sample_positions
namespace gl::amd::framebuffer_sample_positions {
namespace values {
constexpr libc_types::u32 all_pixels                  = 0xFFFFFFFF;
constexpr libc_types::u32 subsample_distance          = 0x883F;
constexpr libc_types::u32 pixels_per_sample_pattern_x = 0x91AE;
constexpr libc_types::u32 pixels_per_sample_pattern_y = 0x91AF;
} // namespace values
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void framebuffer_sample_positionsfv(
        group::framebuffer_target target,
        u32                       numsamples,
        u32                       pixelindex,
        span_const_f32 const&     values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferSamplePositionsfvAMD)
    }
    glFramebufferSamplePositionsfvAMD(
        static_cast<GLenum>(target),
        numsamples,
        pixelindex,
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("FramebufferSamplePositionsfvAMD"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_framebuffer_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment_parameter_name pname,
        u32                                          numsamples,
        u32                                          pixelindex,
        i32                                          size,
        span_f32                                     values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameterfvAMD)
    }
    glGetFramebufferParameterfvAMD(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        numsamples,
        pixelindex,
        size,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetFramebufferParameterfvAMD"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_named_framebuffer_parameter(
        u32      framebuffer,
        GLenum   pname,
        u32      numsamples,
        u32      pixelindex,
        i32      size,
        span_f32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameterfvAMD)
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferParameterfvAMD(
        framebuffer,
        pname,
        numsamples,
        pixelindex,
        size,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameterfvAMD"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void named_framebuffer_sample_positionsfv(
        u32                   framebuffer,
        u32                   numsamples,
        u32                   pixelindex,
        span_const_f32 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferSamplePositionsfvAMD)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferSamplePositionsfvAMD(
        framebuffer,
        numsamples,
        pixelindex,
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("NamedFramebufferSamplePositionsfvAMD"sv);
}

} // namespace gl::amd::framebuffer_sample_positions
#endif // GL_AMD_framebuffer_sample_positions
namespace gl::amd::framebuffer_sample_positions {
constexpr auto name = "GL_AMD_framebuffer_sample_positions";
}
