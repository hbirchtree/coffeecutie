#pragma once

#ifdef GL_EXT_pixel_transform
#include "../enums/PixelTransformPNameEXT.h"
#include "../enums/PixelTransformTargetEXT.h"
namespace gl::ext::pixel_transform {
using gl::group::pixel_transform_prop_ext;
using gl::group::pixel_transform_target_ext;
namespace values {
constexpr libc_types::u32 cubic                              = 0x8334;
constexpr libc_types::u32 average                            = 0x8335;
constexpr libc_types::u32 pixel_transform_2d_stack_depth     = 0x8336;
constexpr libc_types::u32 max_pixel_transform_2d_stack_depth = 0x8337;
constexpr libc_types::u32 pixel_transform_2d_matrix          = 0x8338;
} // namespace values
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_pixel_transform_parameter(
        GLenum target, GLenum pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPixelTransformParameterfvEXT)
    }
    glGetPixelTransformParameterfvEXT(
        target,
        pname,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetPixelTransformParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_pixel_transform_parameter(
        GLenum target, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPixelTransformParameterivEXT)
    }
    glGetPixelTransformParameterivEXT(
        target,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetPixelTransformParameterivEXT"sv);
}

STATICINLINE void pixel_transform_parameter(
    group::pixel_transform_target_ext target,
    group::pixel_transform_prop_ext   pname,
    f32                               param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransformParameterfEXT)
    }
    glPixelTransformParameterfEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("PixelTransformParameterfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void pixel_transform_parameter(
        group::pixel_transform_target_ext target,
        group::pixel_transform_prop_ext   pname,
        span_const_f32 const&             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransformParameterfvEXT)
    }
    glPixelTransformParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("PixelTransformParameterfvEXT"sv);
}

STATICINLINE void pixel_transform_parameter(
    group::pixel_transform_target_ext target,
    group::pixel_transform_prop_ext   pname,
    i32                               param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransformParameteriEXT)
    }
    glPixelTransformParameteriEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("PixelTransformParameteriEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void pixel_transform_parameter(
        group::pixel_transform_target_ext target,
        group::pixel_transform_prop_ext   pname,
        span_const_i32 const&             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransformParameterivEXT)
    }
    glPixelTransformParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("PixelTransformParameterivEXT"sv);
}

} // namespace gl::ext::pixel_transform
#endif // GL_EXT_pixel_transform
namespace gl::ext::pixel_transform {
constexpr auto name = "GL_EXT_pixel_transform";
}
