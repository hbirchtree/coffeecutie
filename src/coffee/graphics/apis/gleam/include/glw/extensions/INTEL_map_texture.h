#pragma once

#ifdef GL_INTEL_map_texture
#include "../enums/MapTextureFormatINTEL.h"
namespace gl::intel::map_texture {
using gl::group::map_texture_format_intel;
namespace values {
constexpr libc_types::u32 texture_memory_layout = 0x83FF;
} // namespace values
STATICINLINE void* map_texture_2d(
    u32 texture, i32 level, GLbitfield access, i32& stride, GLenum& layout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapTexture2DINTEL)
        glIsTexture(texture);
    }
    auto out = glMapTexture2DINTEL(texture, level, access, &stride, &layout);
    detail::error_check("MapTexture2DINTEL"sv);
    return out;
}

STATICINLINE void sync_texture(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SyncTextureINTEL)
        glIsTexture(texture);
    }
    glSyncTextureINTEL(texture);
    detail::error_check("SyncTextureINTEL"sv);
}

STATICINLINE void unmap_texture_2d(u32 texture, i32 level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapTexture2DINTEL)
        glIsTexture(texture);
    }
    glUnmapTexture2DINTEL(texture, level);
    detail::error_check("UnmapTexture2DINTEL"sv);
}

} // namespace gl::intel::map_texture
#endif // GL_INTEL_map_texture
namespace gl::intel::map_texture {
constexpr auto name = "GL_INTEL_map_texture";
}
