#pragma once

#ifdef GL_ARB_get_texture_sub_image
namespace gl::arb::get_texture_sub_image {
namespace values {
} // namespace values
template<class size_3_i32, class span_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_ARB_get_texture_sub_image
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void get_compressed_texture_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        span_void         pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetCompressedTextureSubImage(
        texture,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureSubImage"sv);
}

template<class size_3_i32, class span_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_ARB_get_texture_sub_image
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
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void get_texture_sub_image(
        u32                 texture,
        i32                 level,
        vec_3_i32 const&    xoffset,
        size_3_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureSubImage(
        texture,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureSubImage"sv);
}

} // namespace gl::arb::get_texture_sub_image
#endif // GL_ARB_get_texture_sub_image
namespace gl::arb::get_texture_sub_image {
constexpr auto name = "GL_ARB_get_texture_sub_image";
}
