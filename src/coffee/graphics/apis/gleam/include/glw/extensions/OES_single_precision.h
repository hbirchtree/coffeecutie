#pragma once

#ifdef GL_OES_single_precision
namespace gl::oes::single_precision {
namespace values {
} // namespace values
STATICINLINE void clear_depthf(GLclampf depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthfOES)
    }
    glClearDepthfOES(depth);
    detail::error_check("ClearDepthfOES"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void clip_planef(
        group::clip_plane_name plane, span_const_f32 const& equation)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipPlanefOES)
    }
    glClipPlanefOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<const GLfloat*>(equation.data())
                        : nullptr);
    detail::error_check("ClipPlanefOES"sv);
}

STATICINLINE void depth_rangef(GLclampf n, GLclampf f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangefOES)
    }
    glDepthRangefOES(n, f);
    detail::error_check("DepthRangefOES"sv);
}

STATICINLINE void frustumf(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrustumfOES)
    }
    glFrustumfOES(l, r, b, t, n, f);
    detail::error_check("FrustumfOES"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_clip_planef(group::clip_plane_name plane, span_f32 equation)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetClipPlanefOES)
    }
    glGetClipPlanefOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<GLfloat*>(equation.data())
                        : nullptr);
    detail::error_check("GetClipPlanefOES"sv);
}

STATICINLINE void orthof(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(OrthofOES)
    }
    glOrthofOES(l, r, b, t, n, f);
    detail::error_check("OrthofOES"sv);
}

} // namespace gl::oes::single_precision
#endif // GL_OES_single_precision
namespace gl::oes::single_precision {
constexpr auto name = "GL_OES_single_precision";
}
