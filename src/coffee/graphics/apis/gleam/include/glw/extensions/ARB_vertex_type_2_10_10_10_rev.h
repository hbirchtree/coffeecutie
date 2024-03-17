#pragma once

#ifdef GL_ARB_vertex_type_2_10_10_10_rev
#include "../enums/PixelType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"

namespace gl::arb::vertex_type_2_10_10_10_rev {
using gl::group::pixel_type;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p1ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP1ui)
    }
    glVertexAttribP1ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP1ui"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p1uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_u32 const&             value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP1uiv)
    }
    glVertexAttribP1uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP1uiv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p2ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP2ui)
    }
    glVertexAttribP2ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP2ui"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p2uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_2_u32 const&       value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP2uiv)
    }
    glVertexAttribP2uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP2uiv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p3ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP3ui)
    }
    glVertexAttribP3ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP3ui"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p3uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_3_u32 const&       value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP3uiv)
    }
    glVertexAttribP3uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP3uiv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p4ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP4ui)
    }
    glVertexAttribP4ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP4ui"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p4uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_4_u32 const&       value,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP4uiv)
    }
    glVertexAttribP4uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP4uiv"sv, check_errors);
}

} // namespace gl::arb::vertex_type_2_10_10_10_rev
#endif // GL_ARB_vertex_type_2_10_10_10_rev
namespace gl::arb::vertex_type_2_10_10_10_rev {
constexpr auto name = "GL_ARB_vertex_type_2_10_10_10_rev";
}
