#pragma once

#ifdef GL_EXT_texture3D
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/GetTextureParameter.h"
#include "../enums/PixelStoreParameter.h"
#include "../enums/TextureParameterName.h"
#include "../enums/TextureTarget.h"
namespace gl::ext::texture3d {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::get_texture_parameter;
using gl::group::pixel_store_parameter;
using gl::group::texture_parameter_name;
using gl::group::texture_target;
namespace values {
} // namespace values
template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_image_3d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3DEXT)
    }
    glTexImage3DEXT(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexImage3DEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage3DEXT)
    }
    glTexSubImage3DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage3DEXT"sv);
}

} // namespace gl::ext::texture3d
#endif // GL_EXT_texture3D
namespace gl::ext::texture3d {
constexpr auto name = "GL_EXT_texture3D";
}