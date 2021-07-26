#pragma once

#ifdef GL_EXT_texture_perturb_normal
#include "../enums/TextureNormalModeEXT.h"
namespace gl::ext::texture_perturb_normal {
using gl::group::texture_normal_mode_ext;
namespace values {
constexpr libc_types::u32 texture_normal = 0x85AF;
} // namespace values
STATICINLINE void texture_normal(group::texture_normal_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureNormalEXT)
    }
    glTextureNormalEXT(static_cast<GLenum>(mode));
    detail::error_check("TextureNormalEXT"sv);
}

} // namespace gl::ext::texture_perturb_normal
#endif // GL_EXT_texture_perturb_normal
namespace gl::ext::texture_perturb_normal {
constexpr auto name = "GL_EXT_texture_perturb_normal";
}
