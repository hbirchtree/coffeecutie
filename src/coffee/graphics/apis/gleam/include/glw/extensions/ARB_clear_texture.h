#pragma once

#ifdef GL_ARB_clear_texture
#include "../enums/InternalFormatPName.h"
namespace gl::arb::clear_texture {
using gl::group::internal_format_prop;
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_tex_image(
        u32                    texture,
        i32                    level,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexImage)
        glIsTexture(texture);
    }
    glClearTexImage(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexImage"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_tex_sub_image(
        u32                    texture,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexSubImage)
        glIsTexture(texture);
    }
    glClearTexSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexSubImage"sv);
}

} // namespace gl::arb::clear_texture
#endif // GL_ARB_clear_texture
namespace gl::arb::clear_texture {
constexpr auto name = "GL_ARB_clear_texture";
}