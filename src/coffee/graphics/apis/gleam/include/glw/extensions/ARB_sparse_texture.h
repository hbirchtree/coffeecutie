#pragma once

#ifdef GL_ARB_sparse_texture
namespace gl::arb::sparse_texture {
namespace values {
constexpr u32 virtual_page_size_x                    = 0x9195;
constexpr u32 virtual_page_size_y                    = 0x9196;
constexpr u32 virtual_page_size_z                    = 0x9197;
constexpr u32 max_sparse_texture_size                = 0x9198;
constexpr u32 max_sparse_3d_texture_size             = 0x9199;
constexpr u32 max_sparse_array_texture_layers        = 0x919A;
constexpr u32 texture_sparse                         = 0x91A6;
constexpr u32 virtual_page_size_index                = 0x91A7;
constexpr u32 num_virtual_page_sizes                 = 0x91A8;
constexpr u32 sparse_texture_full_array_cube_mipmaps = 0x91A9;
constexpr u32 num_sparse_levels                      = 0x91AA;
} // namespace values
template<class size_3_i32, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3>&& concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_ARB_sparse_texture
     * \param target GLenum
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param commit GLboolean
     * \return void
     */
    STATICINLINE void tex_page_commitment(
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
        xoffset[0],
        xoffset[1],
        xoffset[2],
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
