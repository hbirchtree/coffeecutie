#pragma once

#ifdef GL_ARB_point_parameters
#include "../enums/PointParameterNameSGIS.h"
namespace gl::arb::point_parameters {
using gl::group::point_parameter_name_sgis;
namespace values {
} // namespace values
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterfARB)
    }
    glPointParameterfARB(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterfARB"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void point_parameter(
        group::point_parameter_name_arb pname, span_const_f32 const& params)
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
    detail::error_check("PointParameterfvARB"sv);
}

} // namespace gl::arb::point_parameters
#endif // GL_ARB_point_parameters
namespace gl::arb::point_parameters {
constexpr auto name = "GL_ARB_point_parameters";
}
