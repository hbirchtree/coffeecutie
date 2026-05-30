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
constexpr u32 texture_application_mode   = 0x834F;
constexpr u32 texture_light              = 0x8350;
constexpr u32 texture_material_face      = 0x8351;
constexpr u32 texture_material_parameter = 0x8352;
} // namespace values

/*!
 * \brief Part of GL_EXT_light_texture
 * \param mode GLenum
 * \return void
 */
STATICINLINE void apply_texture(
    group::light_texture_mode_ext mode,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ApplyTextureEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glApplyTextureEXT, mode);
#endif
    glApplyTextureEXT(static_cast<GLenum>(mode));
    detail::error_check("ApplyTextureEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_light_texture
 * \param pname GLenum
 * \return void
 */
STATICINLINE void texture_light(
    group::light_texture_prop_ext pname,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureLightEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTextureLightEXT, pname);
#endif
    glTextureLightEXT(static_cast<GLenum>(pname));
    detail::error_check("TextureLightEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_light_texture
 * \param face GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void texture_material(
    group::triangle_face      face,
    group::material_parameter mode,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureMaterialEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTextureMaterialEXT, face, mode);
#endif
    glTextureMaterialEXT(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("TextureMaterialEXT"sv, check_errors);
}

} // namespace gl::ext::light_texture
#endif // GL_EXT_light_texture
namespace gl::ext::light_texture {
constexpr auto name = "GL_EXT_light_texture";
}
