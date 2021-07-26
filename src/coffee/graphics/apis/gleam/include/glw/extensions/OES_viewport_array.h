#pragma once

#ifdef GL_OES_viewport_array
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
namespace gl::oes::viewport_array {
using gl::group::enable_cap;
using gl::group::get_prop;
namespace values {
constexpr libc_types::u32 max_viewports                   = 0x825B;
constexpr libc_types::u32 viewport_subpixel_bits          = 0x825C;
constexpr libc_types::u32 viewport_bounds_range           = 0x825D;
constexpr libc_types::u32 viewport_index_provoking_vertex = 0x825F;
} // namespace values
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void depth_range_arrayfv(u32 first, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeArrayfvOES)
    }
    glDepthRangeArrayfvOES(
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("DepthRangeArrayfvOES"sv);
}

STATICINLINE void depth_range_indexedf(u32 index, f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeIndexedfOES)
    }
    glDepthRangeIndexedfOES(index, n, f);
    detail::error_check("DepthRangeIndexedfOES"sv);
}

STATICINLINE void disablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableiOES)
    }
    glDisableiOES(static_cast<GLenum>(target), index);
    detail::error_check("DisableiOES"sv);
}

STATICINLINE void enablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableiOES)
    }
    glEnableiOES(static_cast<GLenum>(target), index);
    detail::error_check("EnableiOES"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_floati_v(group::get_prop target, u32 index, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloati_vOES)
    }
    glGetFloati_vOES(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloati_vOES"sv);
}

STATICINLINE GLboolean is_enabledi(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnablediOES)
    }
    auto out = glIsEnablediOES(static_cast<GLenum>(target), index);
    detail::error_check("IsEnablediOES"sv);
    return out;
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void scissor_arrayv(u32 first, i32 count, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorArrayvOES)
    }
    glScissorArrayvOES(
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorArrayvOES"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void scissor_indexed(
        u32 index, i32 left, i32 bottom, size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexedOES)
    }
    glScissorIndexedOES(index, left, bottom, width[0], width[1]);
    detail::error_check("ScissorIndexedOES"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void scissor_indexedv(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexedvOES)
    }
    glScissorIndexedvOES(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorIndexedvOES"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void viewport_arrayv(u32 first, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportArrayvOES)
    }
    glViewportArrayvOES(
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportArrayvOES"sv);
}

template<class size_2_f32, class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>&&
             semantic::concepts::Size2D<size_2_f32, f32>) STATICINLINE
    void viewport_indexedf(u32 index, vec_2_f32 const& x, size_2_f32 const& w)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedfOES)
    }
    glViewportIndexedfOES(index, x.x(), x.y(), w[0], w[1]);
    detail::error_check("ViewportIndexedfOES"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void viewport_indexedfv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedfvOES)
    }
    glViewportIndexedfvOES(
        index, v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportIndexedfvOES"sv);
}

} // namespace gl::oes::viewport_array
#endif // GL_OES_viewport_array
namespace gl::oes::viewport_array {
constexpr auto name = "GL_OES_viewport_array";
}
