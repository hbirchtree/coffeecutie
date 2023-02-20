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
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP1ui)
    }
    glVertexAttribP1ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP1ui"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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
        span_const_u32 const&             value)
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
    detail::error_check("VertexAttribP1uiv"sv);
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
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP2ui)
    }
    glVertexAttribP2ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP2ui"sv);
}

template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
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
        span_const_vec_2_u32 const&       value)
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
    detail::error_check("VertexAttribP2uiv"sv);
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
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP3ui)
    }
    glVertexAttribP3ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
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
        span_const_vec_3_u32 const&       value)
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
    detail::error_check("VertexAttribP3uiv"sv);
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
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP4ui)
    }
    glVertexAttribP4ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
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
        span_const_vec_4_u32 const&       value)
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
    detail::error_check("VertexAttribP4uiv"sv);
}

#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE void color_p3ui(group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP3ui)
    }
    glColorP3ui(static_cast<GLenum>(type), color);
    detail::error_check("ColorP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param color const GLuint *
     * \return void
     */
    STATICINLINE void color_p3uiv(
        group::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP3uiv)
    }
    glColorP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("ColorP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE void color_p4ui(group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP4ui)
    }
    glColorP4ui(static_cast<GLenum>(type), color);
    detail::error_check("ColorP4ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param color const GLuint *
     * \return void
     */
    STATICINLINE void color_p4uiv(
        group::color_pointer_type type, span_const_vec_4_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP4uiv)
    }
    glColorP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("ColorP4uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p1ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP1ui)
    }
    glMultiTexCoordP1ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP1ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param texture GLenum
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void multi_tex_coord_p1uiv(
        group::texture_unit           texture,
        group::tex_coord_pointer_type type,
        span_const_u32 const&         coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP1uiv)
    }
    glMultiTexCoordP1uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP1uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p2ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP2ui)
    }
    glMultiTexCoordP2ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP2ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param texture GLenum
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void multi_tex_coord_p2uiv(
        group::texture_unit           texture,
        group::tex_coord_pointer_type type,
        span_const_vec_2_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP2uiv)
    }
    glMultiTexCoordP2uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP2uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p3ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP3ui)
    }
    glMultiTexCoordP3ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param texture GLenum
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void multi_tex_coord_p3uiv(
        group::texture_unit           texture,
        group::tex_coord_pointer_type type,
        span_const_vec_3_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP3uiv)
    }
    glMultiTexCoordP3uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p4ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP4ui)
    }
    glMultiTexCoordP4ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP4ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param texture GLenum
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void multi_tex_coord_p4uiv(
        group::texture_unit           texture,
        group::tex_coord_pointer_type type,
        span_const_vec_4_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP4uiv)
    }
    glMultiTexCoordP4uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP4uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void normal_p3ui(group::normal_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalP3ui)
    }
    glNormalP3ui(static_cast<GLenum>(type), coords);
    detail::error_check("NormalP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void normal_p3uiv(
        group::normal_pointer_type type, span_const_vec_3_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalP3uiv)
    }
    glNormalP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("NormalP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE void secondary_color_p3ui(
    group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorP3ui)
    }
    glSecondaryColorP3ui(static_cast<GLenum>(type), color);
    detail::error_check("SecondaryColorP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param color const GLuint *
     * \return void
     */
    STATICINLINE void secondary_color_p3uiv(
        group::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorP3uiv)
    }
    glSecondaryColorP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("SecondaryColorP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p1ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP1ui)
    }
    glTexCoordP1ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP1ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void tex_coord_p1uiv(
        group::tex_coord_pointer_type type, span_const_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP1uiv)
    }
    glTexCoordP1uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP1uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p2ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP2ui)
    }
    glTexCoordP2ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP2ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void tex_coord_p2uiv(
        group::tex_coord_pointer_type type, span_const_vec_2_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP2uiv)
    }
    glTexCoordP2uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP2uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p3ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP3ui)
    }
    glTexCoordP3ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void tex_coord_p3uiv(
        group::tex_coord_pointer_type type, span_const_vec_3_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP3uiv)
    }
    glTexCoordP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p4ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP4ui)
    }
    glTexCoordP4ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP4ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param coords const GLuint *
     * \return void
     */
    STATICINLINE void tex_coord_p4uiv(
        group::tex_coord_pointer_type type, span_const_vec_4_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP4uiv)
    }
    glTexCoordP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP4uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p2ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP2ui)
    }
    glVertexP2ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP2ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void vertex_p2uiv(
        group::vertex_pointer_type type, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP2uiv)
    }
    glVertexP2uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP2uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p3ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP3ui)
    }
    glVertexP3ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP3ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void vertex_p3uiv(
        group::vertex_pointer_type type, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP3uiv)
    }
    glVertexP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP3uiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p4ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP4ui)
    }
    glVertexP4ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP4ui"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_type_2_10_10_10_rev
     * \param type GLenum
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void vertex_p4uiv(
        group::vertex_pointer_type type, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP4uiv)
    }
    glVertexP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP4uiv"sv);
}

#endif
} // namespace gl::arb::vertex_type_2_10_10_10_rev
#endif // GL_ARB_vertex_type_2_10_10_10_rev
namespace gl::arb::vertex_type_2_10_10_10_rev {
constexpr auto name = "GL_ARB_vertex_type_2_10_10_10_rev";
}
