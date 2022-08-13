#pragma once

#ifdef GL_ARB_sparse_texture
namespace gl::arb::sparse_texture {
namespace values {
constexpr libc_types::u32 virtual_page_size_x                    = 0x9195;
constexpr libc_types::u32 virtual_page_size_y                    = 0x9196;
constexpr libc_types::u32 virtual_page_size_z                    = 0x9197;
constexpr libc_types::u32 max_sparse_texture_size                = 0x9198;
constexpr libc_types::u32 max_sparse_3d_texture_size             = 0x9199;
constexpr libc_types::u32 max_sparse_array_texture_layers        = 0x919A;
constexpr libc_types::u32 texture_sparse                         = 0x91A6;
constexpr libc_types::u32 virtual_page_size_index                = 0x91A7;
constexpr libc_types::u32 num_virtual_page_sizes                 = 0x91A8;
constexpr libc_types::u32 sparse_texture_full_array_cube_mipmaps = 0x91A9;
constexpr libc_types::u32 num_sparse_levels                      = 0x91AA;
} // namespace values
template<class size_3_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_page_commitment(
        GLenum            target,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        bool              commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexPageCommitmentARB)
    }
    glTexPageCommitmentARB(
        target,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        commit);
    detail::error_check("TexPageCommitmentARB"sv);
}

} // namespace gl::arb::sparse_texture
#endif // GL_ARB_sparse_texture
namespace gl::arb::sparse_texture {
constexpr auto name = "GL_ARB_sparse_texture";
}