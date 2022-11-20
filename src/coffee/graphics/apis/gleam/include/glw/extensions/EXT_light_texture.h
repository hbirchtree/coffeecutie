#pragma once

#ifdef GL_EXT_light_texture
#include "../enums/FogCoordSrc.h"
#include "../enums/LightTextureModeEXT.h"
#include "../enums/LightTexturePNameEXT.h"
namespace gl::ext::light_texture {
using gl::group::fog_coord_src;
using gl::group::light_texture_mode_ext;
using gl::group::light_texture_prop_ext;
namespace values {
constexpr libc_types::u32 texture_application_mode   = 0x834F;
constexpr libc_types::u32 texture_light              = 0x8350;
constexpr libc_types::u32 texture_material_face      = 0x8351;
constexpr libc_types::u32 texture_material_parameter = 0x8352;
} // namespace values
/*!
 * \brief Part of GL_EXT_light_texture
 * \param mode GLenum
 * \return void
 */
STATICINLINE void apply_texture(group::light_texture_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ApplyTextureEXT)
    }
    glApplyTextureEXT(static_cast<GLenum>(mode));
    detail::error_check("ApplyTextureEXT"sv);
}

/*!
 * \brief Part of GL_EXT_light_texture
 * \param pname GLenum
 * \return void
 */
STATICINLINE void texture_light(group::light_texture_prop_ext pname)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureLightEXT)
    }
    glTextureLightEXT(static_cast<GLenum>(pname));
    detail::error_check("TextureLightEXT"sv);
}

/*!
 * \brief Part of GL_EXT_light_texture
 * \param face GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void texture_material(
    group::triangle_face face, group::material_parameter mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureMaterialEXT)
    }
    glTextureMaterialEXT(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("TextureMaterialEXT"sv);
}

} // namespace gl::ext::light_texture
#endif // GL_EXT_light_texture
namespace gl::ext::light_texture {
constexpr auto name = "GL_EXT_light_texture";
}
