#pragma once

#ifdef GL_EXT_fog_coord
#include "../enums/LightTextureModeEXT.h"
namespace gl::ext::fog_coord {
using gl::group::light_texture_mode_ext;
namespace values {
constexpr libc_types::u32 fog_coordinate_source        = 0x8450;
constexpr libc_types::u32 fog_coordinate               = 0x8451;
constexpr libc_types::u32 current_fog_coordinate       = 0x8453;
constexpr libc_types::u32 fog_coordinate_array_type    = 0x8454;
constexpr libc_types::u32 fog_coordinate_array_stride  = 0x8455;
constexpr libc_types::u32 fog_coordinate_array_pointer = 0x8456;
constexpr libc_types::u32 fog_coordinate_array         = 0x8457;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void fog_coord_pointer(
        group::fog_pointer_type_ext type,
        i32                         stride,
        span_const_void const&      pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordPointerEXT)
    }
    glFogCoordPointerEXT(
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("FogCoordPointerEXT"sv);
}

STATICINLINE void fog_coordd(f64 coord)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoorddEXT)
    }
    glFogCoorddEXT(coord);
    detail::error_check("FogCoorddEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void fog_coorddv(span_const_f64 const& coord)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoorddvEXT)
    }
    glFogCoorddvEXT(
        coord.size() ? reinterpret_cast<const GLdouble*>(coord.data())
                     : nullptr);
    detail::error_check("FogCoorddvEXT"sv);
}

STATICINLINE void fog_coordf(f32 coord)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordfEXT)
    }
    glFogCoordfEXT(coord);
    detail::error_check("FogCoordfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void fog_coordfv(span_const_f32 const& coord)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordfvEXT)
    }
    glFogCoordfvEXT(
        coord.size() ? reinterpret_cast<const GLfloat*>(coord.data())
                     : nullptr);
    detail::error_check("FogCoordfvEXT"sv);
}

} // namespace gl::ext::fog_coord
#endif // GL_EXT_fog_coord
namespace gl::ext::fog_coord {
constexpr auto name = "GL_EXT_fog_coord";
}