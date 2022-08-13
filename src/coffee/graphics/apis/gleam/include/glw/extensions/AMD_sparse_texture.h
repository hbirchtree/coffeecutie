#pragma once

#ifdef GL_AMD_sparse_texture
#include "../enums/TextureStorageMaskAMD.h"
namespace gl::amd::sparse_texture {
using gl::group::texture_storage_mask_amd;
namespace values {
constexpr libc_types::u32 virtual_page_size_x             = 0x9195;
constexpr libc_types::u32 virtual_page_size_y             = 0x9196;
constexpr libc_types::u32 virtual_page_size_z             = 0x9197;
constexpr libc_types::u32 max_sparse_texture_size         = 0x9198;
constexpr libc_types::u32 max_sparse_3d_texture_size      = 0x9199;
constexpr libc_types::u32 max_sparse_array_texture_layers = 0x919A;
constexpr libc_types::u32 min_sparse_level                = 0x919B;
constexpr libc_types::u32 min_lod_warning                 = 0x919C;
} // namespace values
template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_storage_sparse(
        group::texture_target           target,
        group::sized_internal_format    internalFormat,
        size_3_i32 const&               width,
        i32                             layers,
        group::texture_storage_mask_amd flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageSparseAMD)
    }
    glTexStorageSparseAMD(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        layers,
        static_cast<GLenum>(flags));
    detail::error_check("TexStorageSparseAMD"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_storage_sparse(
        u32                             texture,
        GLenum                          target,
        group::sized_internal_format    internalFormat,
        size_3_i32 const&               width,
        i32                             layers,
        group::texture_storage_mask_amd flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageSparseAMD)
        glIsTexture(texture);
    }
    glTextureStorageSparseAMD(
        texture,
        target,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        layers,
        static_cast<GLenum>(flags));
    detail::error_check("TextureStorageSparseAMD"sv);
}

} // namespace gl::amd::sparse_texture
#endif // GL_AMD_sparse_texture
namespace gl::amd::sparse_texture {
constexpr auto name = "GL_AMD_sparse_texture";
}