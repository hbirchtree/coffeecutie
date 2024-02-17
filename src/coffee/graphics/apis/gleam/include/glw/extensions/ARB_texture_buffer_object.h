#pragma once

#ifdef GL_ARB_texture_buffer_object
namespace gl::arb::texture_buffer_object {
namespace values {
constexpr u32 texture_buffer                    = 0x8C2A;
constexpr u32 max_texture_buffer_size           = 0x8C2B;
constexpr u32 texture_binding_buffer            = 0x8C2C;
constexpr u32 texture_buffer_data_store_binding = 0x8C2D;
constexpr u32 texture_buffer_format             = 0x8C2E;
} // namespace values

/*!
 * \brief Part of GL_ARB_texture_buffer_object
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
        GLW_FPTR_CHECK(TexBufferARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTexBufferARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBufferARB"sv);
}

} // namespace gl::arb::texture_buffer_object
#endif // GL_ARB_texture_buffer_object
namespace gl::arb::texture_buffer_object {
constexpr auto name = "GL_ARB_texture_buffer_object";
}
