#pragma once

#ifdef GL_EXT_clear_texture
namespace gl::ext::clear_texture {
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_EXT_clear_texture
     * \param texture GLuint
     * \param level GLint
     * \param format GLenum
     * \param type GLenum
     * \param data const void *
     * \return void
     */
    STATICINLINE void clear_tex_image(
        u32                    texture,
        i32                    level,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexImageEXT)
        glIsTexture(texture);
    }
    glClearTexImageEXT(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexImageEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_EXT_clear_texture
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param data const void *
     * \return void
     */
    STATICINLINE void clear_tex_sub_image(
        u32                    texture,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexSubImageEXT)
        glIsTexture(texture);
    }
    glClearTexSubImageEXT(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexSubImageEXT"sv);
}

} // namespace gl::ext::clear_texture
#endif // GL_EXT_clear_texture
namespace gl::ext::clear_texture {
constexpr auto name = "GL_EXT_clear_texture";
}
