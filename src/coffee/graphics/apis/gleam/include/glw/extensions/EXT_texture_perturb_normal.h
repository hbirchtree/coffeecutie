#pragma once

#ifdef GL_EXT_texture_perturb_normal
#include "../enums/TextureNormalModeEXT.h"

namespace gl::ext::texture_perturb_normal {
using gl::group::texture_normal_mode_ext;

namespace values {
constexpr u32 texture_normal = 0x85AF;
} // namespace values

/*!
 * \brief Part of GL_EXT_texture_perturb_normal
 * \param mode GLenum
 * \return void
 */
STATICINLINE void texture_normal(
    group::texture_normal_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureNormalEXT)
    }
    glTextureNormalEXT(static_cast<GLenum>(mode));
    detail::error_check("TextureNormalEXT"sv, check_errors);
}

} // namespace gl::ext::texture_perturb_normal
#endif // GL_EXT_texture_perturb_normal
namespace gl::ext::texture_perturb_normal {
constexpr auto name = "GL_EXT_texture_perturb_normal";
}
