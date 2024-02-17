#pragma once

#ifdef GL_EXT_vertex_array
#include "../enums/GetPName.h"
#include "../enums/GetPointervPName.h"

namespace gl::ext::vertex_array {
using gl::group::get_pointerv_prop;
using gl::group::get_prop;

namespace values {
constexpr u32 vertex_array               = 0x8074;
constexpr u32 normal_array               = 0x8075;
constexpr u32 color_array                = 0x8076;
constexpr u32 index_array                = 0x8077;
constexpr u32 texture_coord_array        = 0x8078;
constexpr u32 edge_flag_array            = 0x8079;
constexpr u32 vertex_array_size          = 0x807A;
constexpr u32 vertex_array_type          = 0x807B;
constexpr u32 vertex_array_stride        = 0x807C;
constexpr u32 normal_array_type          = 0x807E;
constexpr u32 normal_array_stride        = 0x807F;
constexpr u32 color_array_size           = 0x8081;
constexpr u32 color_array_type           = 0x8082;
constexpr u32 color_array_stride         = 0x8083;
constexpr u32 index_array_type           = 0x8085;
constexpr u32 index_array_stride         = 0x8086;
constexpr u32 texture_coord_array_size   = 0x8088;
constexpr u32 texture_coord_array_type   = 0x8089;
constexpr u32 texture_coord_array_stride = 0x808A;
constexpr u32 edge_flag_array_stride     = 0x808C;
} // namespace values

/*!
 * \brief Part of GL_EXT_vertex_array
 * \param i GLint
 * \return void
 */
STATICINLINE void array_element(i32 i)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ArrayElementEXT)
    }
    glArrayElementEXT(i);
    detail::error_check("ArrayElementEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void color_pointer(
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    i32                       count,
    span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorPointerEXT)
    }
    glColorPointerEXT(
        size,
        static_cast<GLenum>(type),
        stride,
        count,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("ColorPointerEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_array
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \return void
 */
STATICINLINE void draw_arrays(group::primitive_type mode, i32 first, i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysEXT)
    }
    glDrawArraysEXT(static_cast<GLenum>(mode), first, count);
    detail::error_check("DrawArraysEXT"sv);
}

template<class span_const_bool>
requires(
    concepts::span<span_const_bool> &&
    std::is_same_v<
        std::decay_t<typename span_const_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const GLboolean *
 * \return void
 */
STATICINLINE void edge_flag_pointer(
    i32 stride, i32 count, span_const_bool const& pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EdgeFlagPointerEXT)
    }
    glEdgeFlagPointerEXT(
        stride,
        count,
        pointer.size() ? reinterpret_cast<const GLboolean*>(pointer.data())
                       : nullptr);
    detail::error_check("EdgeFlagPointerEXT"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_pointerv(group::get_pointerv_prop pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointervEXT)
    }
    glGetPointervEXT(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointervEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param type GLenum
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void index_pointer(
    group::index_pointer_type type,
    i32                       stride,
    i32                       count,
    span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexPointerEXT)
    }
    glIndexPointerEXT(
        static_cast<GLenum>(type),
        stride,
        count,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("IndexPointerEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param type GLenum
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void normal_pointer(
    group::normal_pointer_type type,
    i32                        stride,
    i32                        count,
    span_const_void const&     pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalPointerEXT)
    }
    glNormalPointerEXT(
        static_cast<GLenum>(type),
        stride,
        count,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("NormalPointerEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void tex_coord_pointer(
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    i32                           count,
    span_const_void const&        pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordPointerEXT)
    }
    glTexCoordPointerEXT(
        size,
        static_cast<GLenum>(type),
        stride,
        count,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("TexCoordPointerEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_array
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param count GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_pointer(
    i32                        size,
    group::vertex_pointer_type type,
    i32                        stride,
    i32                        count,
    span_const_void const&     pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexPointerEXT)
    }
    glVertexPointerEXT(
        size,
        static_cast<GLenum>(type),
        stride,
        count,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexPointerEXT"sv);
}

} // namespace gl::ext::vertex_array
#endif // GL_EXT_vertex_array
namespace gl::ext::vertex_array {
constexpr auto name = "GL_EXT_vertex_array";
}
