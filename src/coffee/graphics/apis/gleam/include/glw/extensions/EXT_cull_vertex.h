#pragma once

#ifdef GL_EXT_cull_vertex
#include "../enums/CullParameterEXT.h"
namespace gl::ext::cull_vertex {
using gl::group::cull_parameter_ext;
namespace values {
constexpr libc_types::u32 cull_vertex = 0x81AA;
} // namespace values
template<class span_f64>
requires(
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_cull_vertex
 * \param pname GLenum
 * \param params GLdouble *
 * \return void
 */
STATICINLINE
    void cull_parameter(group::cull_parameter_ext pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CullParameterdvEXT)
    }
    glCullParameterdvEXT(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("CullParameterdvEXT"sv);
}

template<class span_f32>
requires(
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_cull_vertex
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE
    void cull_parameter(group::cull_parameter_ext pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CullParameterfvEXT)
    }
    glCullParameterfvEXT(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("CullParameterfvEXT"sv);
}

} // namespace gl::ext::cull_vertex
#endif // GL_EXT_cull_vertex
namespace gl::ext::cull_vertex {
constexpr auto name = "GL_EXT_cull_vertex";
}
