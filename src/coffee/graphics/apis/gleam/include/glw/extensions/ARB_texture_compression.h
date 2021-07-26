#pragma once

#ifdef GL_ARB_texture_compression
#include "../enums/HintTarget.h"
namespace gl::arb::texture_compression {
using gl::group::hint_target;
namespace values {
constexpr libc_types::u32 compressed_alpha               = 0x84E9;
constexpr libc_types::u32 compressed_luminance           = 0x84EA;
constexpr libc_types::u32 compressed_luminance_alpha     = 0x84EB;
constexpr libc_types::u32 compressed_intensity           = 0x84EC;
constexpr libc_types::u32 compressed_rgb                 = 0x84ED;
constexpr libc_types::u32 compressed_rgba                = 0x84EE;
constexpr libc_types::u32 texture_compressed_image_size  = 0x86A0;
constexpr libc_types::u32 texture_compressed             = 0x86A1;
constexpr libc_types::u32 num_compressed_texture_formats = 0x86A2;
constexpr libc_types::u32 compressed_texture_formats     = 0x86A3;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_1d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        i32                    width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage1DARB)
    }
    glCompressedTexImage1DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width,
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage1DARB"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_2d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage2DARB)
    }
    glCompressedTexImage2DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage2DARB"sv);
}

template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_3d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage3DARB)
    }
    glCompressedTexImage3DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage3DARB"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_1d(
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage1DARB)
    }
    glCompressedTexSubImage1DARB(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage1DARB"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_2d(
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage2DARB)
    }
    glCompressedTexSubImage2DARB(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage2DARB"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage3DARB)
    }
    glCompressedTexSubImage3DARB(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage3DARB"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_tex_image(
        group::texture_target target, i32 level, span_void img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTexImageARB)
    }
    glGetCompressedTexImageARB(
        static_cast<GLenum>(target),
        level,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedTexImageARB"sv);
}

} // namespace gl::arb::texture_compression
#endif // GL_ARB_texture_compression
namespace gl::arb::texture_compression {
constexpr auto name = "GL_ARB_texture_compression";
}
