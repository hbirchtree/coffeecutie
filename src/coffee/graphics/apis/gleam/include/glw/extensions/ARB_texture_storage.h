#pragma once

#ifdef GL_ARB_texture_storage
#include "../enums/GetTextureParameter.h"
namespace gl::arb::texture_storage {
using gl::group::get_texture_parameter;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_texture_storage
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \return void
 */
STATICINLINE void tex_storage_1d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage1D)
    }
    glTexStorage1D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width);
    detail::error_check("TexStorage1D"sv);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
    /*!
     * \brief Part of GL_ARB_texture_storage
     * \param target GLenum
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void tex_storage_2d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2D)
    }
    glTexStorage2D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TexStorage2D"sv);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_ARB_texture_storage
     * \param target GLenum
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \return void
     */
    STATICINLINE void tex_storage_3d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3D)
    }
    glTexStorage3D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TexStorage3D"sv);
}

} // namespace gl::arb::texture_storage
#endif // GL_ARB_texture_storage
namespace gl::arb::texture_storage {
constexpr auto name = "GL_ARB_texture_storage";
}
