#pragma once

#ifdef GL_EXT_texture_array
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/TextureTarget.h"
namespace gl::ext::texture_array {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 compare_ref_depth_to_texture = 0x884E;
constexpr libc_types::u32 max_array_texture_layers     = 0x88FF;
constexpr libc_types::u32 texture_1d_array             = 0x8C18;
constexpr libc_types::u32 texture_2d_array             = 0x8C1A;
constexpr libc_types::u32 texture_binding_1d_array     = 0x8C1C;
constexpr libc_types::u32 texture_binding_2d_array     = 0x8C1D;
} // namespace values
/*!
 * \brief Part of GL_EXT_texture_array
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_layer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayerEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureLayerEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayerEXT"sv);
}

} // namespace gl::ext::texture_array
#endif // GL_EXT_texture_array
namespace gl::ext::texture_array {
constexpr auto name = "GL_EXT_texture_array";
}
