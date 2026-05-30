#pragma once

#ifdef GL_OES_single_precision
namespace gl::oes::single_precision {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_OES_single_precision
 * \param depth GLclampf
 * \return void
 */
STATICINLINE void clear_depthf(
    GLclampf depth, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthfOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearDepthfOES, depth);
#endif
    glClearDepthfOES(depth);
    detail::error_check("ClearDepthfOES"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_OES_single_precision
 * \param plane GLenum
 * \param equation const GLfloat *
 * \return void
 */
STATICINLINE void clip_planef(
    group::clip_plane_name plane,
    span_const_f32 const&  equation,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipPlanefOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glClipPlanefOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(equation.data()),
            equation.size_bytes()),
        plane);
#endif
    glClipPlanefOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<const GLfloat*>(equation.data())
                        : nullptr);
    detail::error_check("ClipPlanefOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_single_precision
 * \param n GLclampf
 * \param f GLclampf
 * \return void
 */
STATICINLINE void depth_rangef(
    GLclampf n, GLclampf f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangefOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDepthRangefOES, n, f);
#endif
    glDepthRangefOES(n, f);
    detail::error_check("DepthRangefOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_single_precision
 * \param l GLfloat
 * \param r GLfloat
 * \param b GLfloat
 * \param t GLfloat
 * \param n GLfloat
 * \param f GLfloat
 * \return void
 */
STATICINLINE void frustumf(
    f32         l,
    f32         r,
    f32         b,
    f32         t,
    f32         n,
    f32         f,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrustumfOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFrustumfOES, l, r, b, t, n, f);
#endif
    glFrustumfOES(l, r, b, t, n, f);
    detail::error_check("FrustumfOES"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_OES_single_precision
 * \param plane GLenum
 * \param equation GLfloat *
 * \return void
 */
STATICINLINE void get_clip_planef(
    group::clip_plane_name plane,
    span_f32               equation,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetClipPlanefOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetClipPlanefOES,
        gsl::span<char>(
            reinterpret_cast<char*>(equation.data()), equation.size_bytes()),
        plane);
#endif
    glGetClipPlanefOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<GLfloat*>(equation.data())
                        : nullptr);
    detail::error_check("GetClipPlanefOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_single_precision
 * \param l GLfloat
 * \param r GLfloat
 * \param b GLfloat
 * \param t GLfloat
 * \param n GLfloat
 * \param f GLfloat
 * \return void
 */
STATICINLINE void orthof(
    f32         l,
    f32         r,
    f32         b,
    f32         t,
    f32         n,
    f32         f,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(OrthofOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glOrthofOES, l, r, b, t, n, f);
#endif
    glOrthofOES(l, r, b, t, n, f);
    detail::error_check("OrthofOES"sv, check_errors);
}

} // namespace gl::oes::single_precision
#endif // GL_OES_single_precision
namespace gl::oes::single_precision {
constexpr auto name = "GL_OES_single_precision";
}
