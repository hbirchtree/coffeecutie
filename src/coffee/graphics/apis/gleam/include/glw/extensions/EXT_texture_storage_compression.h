#pragma once

#ifdef GL_EXT_texture_storage_compression
#include "../enums/GetTextureParameter.h"
#include "../enums/InternalFormatPName.h"
#include "../enums/TexStorageAttribs.h"
namespace gl::ext::texture_storage_compression {
using gl::group::get_texture_parameter;
using gl::group::internal_format_prop;
using gl::group::tex_storage_attribs;
namespace values {
} // namespace values
template<class size_2_i32, class span_const_i32>
requires(concepts::size_2d<size_2_i32, i32>&& concepts::span<span_const_i32>&&
                                              std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_storage_compression
     * \param target GLenum
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param attrib_list const GLint *
     * \return void
     */
    STATICINLINE void tex_storage_attribs_2d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        span_const_i32 const&        attrib_list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageAttribs2DEXT)
    }
    glTexStorageAttribs2DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        attrib_list.size() ? reinterpret_cast<const GLint*>(attrib_list.data())
                           : nullptr);
    detail::error_check("TexStorageAttribs2DEXT"sv);
}

template<class size_3_i32, class span_const_i32>
requires(concepts::size_2d<size_3_i32, i32>&& concepts::span<span_const_i32>&&
                                              std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_storage_compression
     * \param target GLenum
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param attrib_list const GLint *
     * \return void
     */
    STATICINLINE void tex_storage_attribs_3d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width,
        span_const_i32 const&        attrib_list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageAttribs3DEXT)
    }
    glTexStorageAttribs3DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        attrib_list.size() ? reinterpret_cast<const GLint*>(attrib_list.data())
                           : nullptr);
    detail::error_check("TexStorageAttribs3DEXT"sv);
}

} // namespace gl::ext::texture_storage_compression
#endif // GL_EXT_texture_storage_compression
namespace gl::ext::texture_storage_compression {
constexpr auto name = "GL_EXT_texture_storage_compression";
}
