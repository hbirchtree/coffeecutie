#pragma once

#ifdef GL_ARB_texture_buffer_range
#include "../enums/GetPName.h"
namespace gl::arb::texture_buffer_range {
using gl::group::get_prop;
namespace values {
constexpr u32 texture_buffer_offset = 0x919D;
constexpr u32 texture_buffer_size   = 0x919E;
} // namespace values
/*!
 * \brief Part of GL_ARB_texture_buffer_range
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
        GLW_FPTR_CHECK(TexBufferRange)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTexBufferRange(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TexBufferRange"sv);
}

} // namespace gl::arb::texture_buffer_range
#endif // GL_ARB_texture_buffer_range
namespace gl::arb::texture_buffer_range {
constexpr auto name = "GL_ARB_texture_buffer_range";
}
