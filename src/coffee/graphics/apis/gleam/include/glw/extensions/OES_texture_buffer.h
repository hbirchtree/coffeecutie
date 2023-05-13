#pragma once

#ifdef GL_OES_texture_buffer
namespace gl::oes::texture_buffer {
namespace values {
constexpr u32 texture_buffer                    = 0x8C2A;
constexpr u32 texture_buffer_binding            = 0x8C2A;
constexpr u32 max_texture_buffer_size           = 0x8C2B;
constexpr u32 texture_binding_buffer            = 0x8C2C;
constexpr u32 texture_buffer_data_store_binding = 0x8C2D;
constexpr u32 sampler_buffer                    = 0x8DC2;
constexpr u32 int_sampler_buffer                = 0x8DD0;
constexpr u32 unsigned_int_sampler_buffer       = 0x8DD8;
constexpr u32 image_buffer                      = 0x9051;
constexpr u32 int_image_buffer                  = 0x905C;
constexpr u32 unsigned_int_image_buffer         = 0x9067;
constexpr u32 texture_buffer_offset             = 0x919D;
constexpr u32 texture_buffer_size               = 0x919E;
constexpr u32 texture_buffer_offset_alignment   = 0x919F;
} // namespace values
/*!
 * \brief Part of GL_OES_texture_buffer
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void tex_buffer(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBufferOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTexBufferOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBufferOES"sv);
}

/*!
 * \brief Part of GL_OES_texture_buffer
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void tex_buffer_range(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    GLintptr                     offset,
    GLsizeiptr                   size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBufferRangeOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTexBufferRangeOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TexBufferRangeOES"sv);
}

} // namespace gl::oes::texture_buffer
#endif // GL_OES_texture_buffer
namespace gl::oes::texture_buffer {
constexpr auto name = "GL_OES_texture_buffer";
}
