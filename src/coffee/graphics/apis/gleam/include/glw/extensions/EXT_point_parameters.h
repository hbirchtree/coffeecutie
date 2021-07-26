#pragma once

#ifdef GL_EXT_point_parameters
#include "../enums/PointParameterNameARB.h"
#include "../enums/PointParameterNameSGIS.h"
namespace gl::ext::point_parameters {
using gl::group::point_parameter_name_arb;
using gl::group::point_parameter_name_sgis;
namespace values {
} // namespace values
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterfEXT)
    }
    glPointParameterfEXT(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterfEXT"sv);
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
        GLW_FPTR_CHECK(PointParameterfvEXT)
    }
    glPointParameterfvEXT(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterfvEXT"sv);
}

} // namespace gl::ext::point_parameters
#endif // GL_EXT_point_parameters
namespace gl::ext::point_parameters {
constexpr auto name = "GL_EXT_point_parameters";
}
