#pragma once

#ifdef GL_INTEL_map_texture
#include "../enums/MapTextureFormatINTEL.h"
#include "../enums/TextureParameterName.h"
namespace gl::intel::map_texture {
using gl::group::map_texture_format_intel;
using gl::group::texture_parameter_name;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_INTEL_map_texture
 * \param texture GLuint
 * \param level GLint
 * \param access GLbitfield
 * \param stride GLint *
 * \param layout GLenum *
 * \return void *
 */
STATICINLINE void* map_texture_2d(
    u32 texture, i32 level, GLbitfield access, i32& stride, GLenum& layout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapTexture2DINTEL)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    auto out = glMapTexture2DINTEL(texture, level, access, &stride, &layout);
    detail::error_check("MapTexture2DINTEL"sv);
    return out;
}

/*!
 * \brief Part of GL_INTEL_map_texture
 * \param texture GLuint
 * \return void
 */
STATICINLINE void sync_texture(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SyncTextureINTEL)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glSyncTextureINTEL(texture);
    detail::error_check("SyncTextureINTEL"sv);
}

/*!
 * \brief Part of GL_INTEL_map_texture
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void unmap_texture_2d(u32 texture, i32 level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapTexture2DINTEL)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glUnmapTexture2DINTEL(texture, level);
    detail::error_check("UnmapTexture2DINTEL"sv);
}

} // namespace gl::intel::map_texture
#endif // GL_INTEL_map_texture
namespace gl::intel::map_texture {
constexpr auto name = "GL_INTEL_map_texture";
}
