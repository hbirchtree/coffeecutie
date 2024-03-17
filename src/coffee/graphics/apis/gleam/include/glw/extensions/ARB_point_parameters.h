#pragma once

#ifdef GL_ARB_point_parameters
#include "../enums/GetPName.h"
#include "../enums/PointParameterNameARB.h"

namespace gl::arb::point_parameters {
using gl::group::get_prop;
using gl::group::point_parameter_name_arb;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_point_parameters
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    f32                             param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterfARB)
    }
    glPointParameterfARB(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterfARB"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_point_parameters
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    span_const_f32 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterfvARB)
    }
    glPointParameterfvARB(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterfvARB"sv, check_errors);
}

} // namespace gl::arb::point_parameters
#endif // GL_ARB_point_parameters
namespace gl::arb::point_parameters {
constexpr auto name = "GL_ARB_point_parameters";
}
