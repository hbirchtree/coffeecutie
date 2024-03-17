#pragma once

#ifdef GL_EXT_multiview_draw_buffers
#include "../enums/GetPName.h"

namespace gl::ext::multiview_draw_buffers {
using gl::group::get_prop;

namespace values {
constexpr u32 color_attachment      = 0x90F0;
constexpr u32 multiview             = 0x90F1;
constexpr u32 max_multiview_buffers = 0x90F2;
} // namespace values

template<class span_const_GLenum, class span_const_i32>
requires(
    concepts::span<span_const_GLenum> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLenum::value_type>,
        std::decay_t<GLenum>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_multiview_draw_buffers
 * \param n GLint
 * \param location const GLenum *
 * \param indices const GLint *
 * \return void
 */
STATICINLINE void draw_buffers_indexed(
    span_const_GLenum const& location,
    span_const_i32 const&    indices,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffersIndexedEXT)
    }
    glDrawBuffersIndexedEXT(
        location.size(),
        location.size() ? reinterpret_cast<const GLenum*>(location.data())
                        : nullptr,
        indices.size() ? reinterpret_cast<const GLint*>(indices.data())
                       : nullptr);
    detail::error_check("DrawBuffersIndexedEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_multiview_draw_buffers
 * \param target GLenum
 * \param index GLuint
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_integeri_v(
    group::get_prop target,
    u32             index,
    span_i32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegeri_vEXT)
    }
    glGetIntegeri_vEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegeri_vEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_multiview_draw_buffers
 * \param src GLenum
 * \param index GLint
 * \return void
 */
STATICINLINE void read_buffer_indexed(
    group::read_buffer_mode src,
    i32                     index,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadBufferIndexedEXT)
    }
    glReadBufferIndexedEXT(static_cast<GLenum>(src), index);
    detail::error_check("ReadBufferIndexedEXT"sv, check_errors);
}

} // namespace gl::ext::multiview_draw_buffers
#endif // GL_EXT_multiview_draw_buffers
namespace gl::ext::multiview_draw_buffers {
constexpr auto name = "GL_EXT_multiview_draw_buffers";
}
