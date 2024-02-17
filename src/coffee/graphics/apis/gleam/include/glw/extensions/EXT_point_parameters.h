#pragma once

#ifdef GL_EXT_point_parameters
#include "../enums/GetPName.h"
#include "../enums/PointParameterNameARB.h"

namespace gl::ext::point_parameters {
using gl::group::get_prop;
using gl::group::point_parameter_name_arb;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_point_parameters
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
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
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_point_parameters
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void point_parameter(
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
