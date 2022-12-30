#pragma once

#ifdef GL_EXT_texture_integer
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_integer {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 red_integer             = 0x8D94;
constexpr libc_types::u32 green_integer           = 0x8D95;
constexpr libc_types::u32 blue_integer            = 0x8D96;
constexpr libc_types::u32 alpha_integer           = 0x8D97;
constexpr libc_types::u32 rgb_integer             = 0x8D98;
constexpr libc_types::u32 rgba_integer            = 0x8D99;
constexpr libc_types::u32 bgr_integer             = 0x8D9A;
constexpr libc_types::u32 bgra_integer            = 0x8D9B;
constexpr libc_types::u32 luminance_integer       = 0x8D9C;
constexpr libc_types::u32 luminance_alpha_integer = 0x8D9D;
constexpr libc_types::u32 rgba_integer_mode       = 0x8D9E;
} // namespace values
template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param red GLint
 * \param green GLint
 * \param blue GLint
 * \param alpha GLint
 * \return void
 */
STATICINLINE void clear_color_ii(vec_4_i32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColorIiEXT)
    }
    glClearColorIiEXT(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ClearColorIiEXT"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param red GLuint
 * \param green GLuint
 * \param blue GLuint
 * \param alpha GLuint
 * \return void
 */
STATICINLINE void clear_color_iui(vec_4_u32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColorIuiEXT)
    }
    glClearColorIuiEXT(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ClearColorIuiEXT"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIivEXT)
    }
    glGetTexParameterIivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIivEXT"sv);
}

template<class span_u32>
requires(
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iuiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuivEXT)
    }
    glGetTexParameterIuivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuivEXT"sv);
}

template<class span_const_i32>
requires(
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void tex_parameter_iiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIivEXT)
    }
    glTexParameterIivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIivEXT"sv);
}

template<class span_const_u32>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_texture_integer
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void tex_parameter_iuiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuivEXT)
    }
    glTexParameterIuivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuivEXT"sv);
}

} // namespace gl::ext::texture_integer
#endif // GL_EXT_texture_integer
namespace gl::ext::texture_integer {
constexpr auto name = "GL_EXT_texture_integer";
}
