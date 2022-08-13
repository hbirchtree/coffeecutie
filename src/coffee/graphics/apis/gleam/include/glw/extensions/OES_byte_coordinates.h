#pragma once

#ifdef GL_OES_byte_coordinates
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/ColorPointerType.h"
#include "../enums/ListNameType.h"
#include "../enums/NormalPointerType.h"
#include "../enums/PixelType.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/VertexAttribIType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/WeightPointerTypeARB.h"
namespace gl::oes::byte_coordinates {
using gl::group::binormal_pointer_type_ext;
using gl::group::color_pointer_type;
using gl::group::list_name_type;
using gl::group::normal_pointer_type;
using gl::group::pixel_type;
using gl::group::tangent_pointer_type_ext;
using gl::group::vertex_attrib_int;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::weight_pointer_type_arb;
namespace values {
} // namespace values
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord1b(group::texture_unit texture, i8 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1bOES)
    }
    glMultiTexCoord1bOES(static_cast<GLenum>(texture), s);
    detail::error_check("MultiTexCoord1bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void multi_tex_coord1bv(
        group::texture_unit texture, span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1bvOES)
    }
    glMultiTexCoord1bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord1bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord2b(group::texture_unit texture, i8 s, i8 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2bOES)
    }
    glMultiTexCoord2bOES(static_cast<GLenum>(texture), s, t);
    detail::error_check("MultiTexCoord2bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void multi_tex_coord2bv(
        group::texture_unit texture, span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2bvOES)
    }
    glMultiTexCoord2bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord2bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord3b(
    group::texture_unit texture, i8 s, i8 t, i8 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3bOES)
    }
    glMultiTexCoord3bOES(static_cast<GLenum>(texture), s, t, r);
    detail::error_check("MultiTexCoord3bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void multi_tex_coord3bv(
        group::texture_unit texture, span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3bvOES)
    }
    glMultiTexCoord3bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord3bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord4b(
    group::texture_unit texture, i8 s, i8 t, i8 r, i8 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4bOES)
    }
    glMultiTexCoord4bOES(static_cast<GLenum>(texture), s, t, r, q);
    detail::error_check("MultiTexCoord4bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void multi_tex_coord4bv(
        group::texture_unit texture, span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4bvOES)
    }
    glMultiTexCoord4bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord4bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord1b(i8 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1bOES)
    }
    glTexCoord1bOES(s);
    detail::error_check("TexCoord1bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void tex_coord1bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1bvOES)
    }
    glTexCoord1bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord1bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord2b(i8 s, i8 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2bOES)
    }
    glTexCoord2bOES(s, t);
    detail::error_check("TexCoord2bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void tex_coord2bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2bvOES)
    }
    glTexCoord2bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord2bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord3b(i8 s, i8 t, i8 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3bOES)
    }
    glTexCoord3bOES(s, t, r);
    detail::error_check("TexCoord3bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void tex_coord3bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3bvOES)
    }
    glTexCoord3bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord3bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord4b(i8 s, i8 t, i8 r, i8 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4bOES)
    }
    glTexCoord4bOES(s, t, r, q);
    detail::error_check("TexCoord4bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void tex_coord4bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4bvOES)
    }
    glTexCoord4bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord4bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_i8>
requires(semantic::concepts::Vector<vec_2_i8, i8, 2>) STATICINLINE
    void vertex2b(vec_2_i8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2bOES)
    }
    glVertex2bOES(x.x(), x.y());
    detail::error_check("Vertex2bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void vertex2bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2bvOES)
    }
    glVertex2bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex2bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_i8>
requires(semantic::concepts::Vector<vec_3_i8, i8, 3>) STATICINLINE
    void vertex3b(vec_3_i8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3bOES)
    }
    glVertex3bOES(x.x(), x.y(), x.z());
    detail::error_check("Vertex3bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void vertex3bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3bvOES)
    }
    glVertex3bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex3bvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_i8>
requires(semantic::concepts::Vector<vec_4_i8, i8, 4>) STATICINLINE
    void vertex4b(vec_4_i8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4bOES)
    }
    glVertex4bOES(x.x(), x.y(), x.z(), x.w());
    detail::error_check("Vertex4bOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void vertex4bv(span_const_i8 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4bvOES)
    }
    glVertex4bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex4bvOES"sv);
}

#endif
} // namespace gl::oes::byte_coordinates
#endif // GL_OES_byte_coordinates
namespace gl::oes::byte_coordinates {
constexpr auto name = "GL_OES_byte_coordinates";
}