#pragma once

#ifdef GL_EXT_texture_storage
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_storage {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 alpha32f                 = 0x8816;
constexpr libc_types::u32 luminance32f             = 0x8818;
constexpr libc_types::u32 luminance_alpha32f       = 0x8819;
constexpr libc_types::u32 alpha16f                 = 0x881C;
constexpr libc_types::u32 luminance16f             = 0x881E;
constexpr libc_types::u32 luminance_alpha16f       = 0x881F;
constexpr libc_types::u32 texture_immutable_format = 0x912F;
constexpr libc_types::u32 bgra8                    = 0x93A1;
} // namespace values
/*!
 * \brief Part of GL_EXT_texture_storage
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
        GLW_FPTR_CHECK(TexStorage1DEXT)
    }
    glTexStorage1DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width);
    detail::error_check("TexStorage1DEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_texture_storage
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
        GLW_FPTR_CHECK(TexStorage2DEXT)
    }
    glTexStorage2DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TexStorage2DEXT"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_texture_storage
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
        GLW_FPTR_CHECK(TexStorage3DEXT)
    }
    glTexStorage3DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TexStorage3DEXT"sv);
}

/*!
 * \brief Part of GL_EXT_texture_storage
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \return void
 */
STATICINLINE void texture_storage_1d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage1DEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage1DEXT(
        texture, target, levels, static_cast<GLenum>(internalformat), width);
    detail::error_check("TextureStorage1DEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_texture_storage
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void texture_storage_2d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage2DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TextureStorage2DEXT"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_texture_storage
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \return void
 */
STATICINLINE void texture_storage_3d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage3DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TextureStorage3DEXT"sv);
}

} // namespace gl::ext::texture_storage
#endif // GL_EXT_texture_storage
namespace gl::ext::texture_storage {
constexpr auto name = "GL_EXT_texture_storage";
}
