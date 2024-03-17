#pragma once

#ifdef GL_ARB_window_pos
namespace gl::arb::window_pos {
namespace values {
} // namespace values

template<class vec_2_f64>
requires(concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void window_pos2d(
    vec_2_f64 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2dARB)
    }
    glWindowPos2dARB(x[0], x[1]);
    detail::error_check("WindowPos2dARB"sv, check_errors);
}

template<class span_const_vec_2_f64>
requires(
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void window_pos2dv(
    span_const_vec_2_f64 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2dvARB)
    }
    glWindowPos2dvARB(reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("WindowPos2dvARB"sv, check_errors);
}

template<class vec_2_f32>
requires(concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLfloat
 * \param y GLfloat
 * \return void
 */
STATICINLINE void window_pos2f(
    vec_2_f32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2fARB)
    }
    glWindowPos2fARB(x[0], x[1]);
    detail::error_check("WindowPos2fARB"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void window_pos2fv(
    span_const_vec_2_f32 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2fvARB)
    }
    glWindowPos2fvARB(reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("WindowPos2fvARB"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLint
 * \param y GLint
 * \return void
 */
STATICINLINE void window_pos2i(
    vec_2_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2iARB)
    }
    glWindowPos2iARB(x[0], x[1]);
    detail::error_check("WindowPos2iARB"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLint *
 * \return void
 */
STATICINLINE void window_pos2iv(
    span_const_vec_2_i32 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2ivARB)
    }
    glWindowPos2ivARB(reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("WindowPos2ivARB"sv, check_errors);
}

template<class vec_2_i16>
requires(concepts::vector<vec_2_i16, i16, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLshort
 * \param y GLshort
 * \return void
 */
STATICINLINE void window_pos2s(
    vec_2_i16 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2sARB)
    }
    glWindowPos2sARB(x[0], x[1]);
    detail::error_check("WindowPos2sARB"sv, check_errors);
}

template<class span_const_vec_2_i16>
requires(
    concepts::span<span_const_vec_2_i16> &&
    concepts::vector<typename span_const_vec_2_i16::value_type, i16, 2>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void window_pos2sv(
    span_const_vec_2_i16 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos2svARB)
    }
    glWindowPos2svARB(reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("WindowPos2svARB"sv, check_errors);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void window_pos3d(
    vec_3_f64 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3dARB)
    }
    glWindowPos3dARB(x[0], x[1], x[2]);
    detail::error_check("WindowPos3dARB"sv, check_errors);
}

template<class span_const_vec_3_f64>
requires(
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void window_pos3dv(
    span_const_vec_3_f64 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3dvARB)
    }
    glWindowPos3dvARB(reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("WindowPos3dvARB"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void window_pos3f(
    vec_3_f32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3fARB)
    }
    glWindowPos3fARB(x[0], x[1], x[2]);
    detail::error_check("WindowPos3fARB"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void window_pos3fv(
    span_const_vec_3_f32 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3fvARB)
    }
    glWindowPos3fvARB(reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("WindowPos3fvARB"sv, check_errors);
}

template<class vec_3_i32>
requires(concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLint
 * \param y GLint
 * \param z GLint
 * \return void
 */
STATICINLINE void window_pos3i(
    vec_3_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3iARB)
    }
    glWindowPos3iARB(x[0], x[1], x[2]);
    detail::error_check("WindowPos3iARB"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLint *
 * \return void
 */
STATICINLINE void window_pos3iv(
    span_const_vec_3_i32 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3ivARB)
    }
    glWindowPos3ivARB(reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("WindowPos3ivARB"sv, check_errors);
}

template<class vec_3_i16>
requires(concepts::vector<vec_3_i16, i16, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param x GLshort
 * \param y GLshort
 * \param z GLshort
 * \return void
 */
STATICINLINE void window_pos3s(
    vec_3_i16 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3sARB)
    }
    glWindowPos3sARB(x[0], x[1], x[2]);
    detail::error_check("WindowPos3sARB"sv, check_errors);
}

template<class span_const_vec_3_i16>
requires(
    concepts::span<span_const_vec_3_i16> &&
    concepts::vector<typename span_const_vec_3_i16::value_type, i16, 3>)
/*!
 * \brief Part of GL_ARB_window_pos
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void window_pos3sv(
    span_const_vec_3_i16 const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowPos3svARB)
    }
    glWindowPos3svARB(reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("WindowPos3svARB"sv, check_errors);
}

} // namespace gl::arb::window_pos
#endif // GL_ARB_window_pos
namespace gl::arb::window_pos {
constexpr auto name = "GL_ARB_window_pos";
}
