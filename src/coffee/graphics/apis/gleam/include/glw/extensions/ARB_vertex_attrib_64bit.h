#pragma once

#ifdef GL_ARB_vertex_attrib_64bit
#include "../enums/AttributeType.h"
#include "../enums/GlslTypeToken.h"
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/UniformType.h"
namespace gl::arb::vertex_attrib_64bit {
using gl::group::attribute_type;
using gl::group::glsl_type_token;
using gl::group::internal_format;
using gl::group::sized_internal_format;
using gl::group::uniform_type;
namespace values {
} // namespace values
template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_vertex_attrib_ldv(
        u32 index, group::vertex_attrib_enum pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLdv)
    }
    glGetVertexAttribLdv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribLdv"sv);
}

STATICINLINE void vertex_attrib_l1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1d)
    }
    glVertexAttribL1d(index, x);
    detail::error_check("VertexAttribL1d"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void vertex_attrib_l1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1dv)
    }
    glVertexAttribL1dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL1dv"sv);
}

template<class vec_2_f64>
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void vertex_attrib_l2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2d)
    }
    glVertexAttribL2d(index, x.x(), x.y());
    detail::error_check("VertexAttribL2d"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE
    void vertex_attrib_l2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2dv)
    }
    glVertexAttribL2dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL2dv"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void vertex_attrib_l3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3d)
    }
    glVertexAttribL3d(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribL3d"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE
    void vertex_attrib_l3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3dv)
    }
    glVertexAttribL3dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL3dv"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void vertex_attrib_l4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4d)
    }
    glVertexAttribL4d(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribL4d"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE
    void vertex_attrib_l4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4dv)
    }
    glVertexAttribL4dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL4dv"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_l_pointer(
        u32                       index,
        group::vertex_attrib_long type,
        i32                       stride,
        span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLPointer)
    }
    glVertexAttribLPointer(
        index,
        pointer.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribLPointer"sv);
}

} // namespace gl::arb::vertex_attrib_64bit
#endif // GL_ARB_vertex_attrib_64bit
namespace gl::arb::vertex_attrib_64bit {
constexpr auto name = "GL_ARB_vertex_attrib_64bit";
}