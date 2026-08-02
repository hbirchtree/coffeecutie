#pragma once

#ifdef GL_NV_point_sprite
namespace gl::nv::point_sprite {
namespace values {
constexpr u32 point_sprite        = 0x8861;
constexpr u32 coord_replace       = 0x8862;
constexpr u32 point_sprite_r_mode = 0x8863;
} // namespace values

/*!
 * \brief Part of GL_NV_point_sprite
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    i32                             param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameteriNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPointParameteriNV, pname, param);
#endif
    glPointParameteriNV(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameteriNV"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_point_sprite
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    span_const_i32 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameterivNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glPointParameterivNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glPointParameterivNV(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterivNV"sv, check_errors);
}

} // namespace gl::nv::point_sprite
#endif // GL_NV_point_sprite
namespace gl::nv::point_sprite {
constexpr auto name = "GL_NV_point_sprite";
}
