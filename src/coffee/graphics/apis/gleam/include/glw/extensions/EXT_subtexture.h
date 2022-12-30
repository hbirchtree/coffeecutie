#pragma once

#ifdef GL_EXT_subtexture
namespace gl::ext::subtexture {
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_EXT_subtexture
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_1d(
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage1DEXT)
    }
    glTexSubImage1DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Size2D<size_2_i32, i32> &&
    semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_EXT_subtexture
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage2DEXT)
    }
    glTexSubImage2DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2DEXT"sv);
}

} // namespace gl::ext::subtexture
#endif // GL_EXT_subtexture
namespace gl::ext::subtexture {
constexpr auto name = "GL_EXT_subtexture";
}
