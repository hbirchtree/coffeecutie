#pragma once

#ifdef GL_EXT_coordinate_frame
namespace gl::ext::coordinate_frame {
namespace values {
constexpr libc_types::u32 tangent_array          = 0x8439;
constexpr libc_types::u32 binormal_array         = 0x843A;
constexpr libc_types::u32 current_tangent        = 0x843B;
constexpr libc_types::u32 current_binormal       = 0x843C;
constexpr libc_types::u32 tangent_array_type     = 0x843E;
constexpr libc_types::u32 tangent_array_stride   = 0x843F;
constexpr libc_types::u32 binormal_array_type    = 0x8440;
constexpr libc_types::u32 binormal_array_stride  = 0x8441;
constexpr libc_types::u32 tangent_array_pointer  = 0x8442;
constexpr libc_types::u32 binormal_array_pointer = 0x8443;
constexpr libc_types::u32 map1_tangent           = 0x8444;
constexpr libc_types::u32 map2_tangent           = 0x8445;
constexpr libc_types::u32 map1_binormal          = 0x8446;
constexpr libc_types::u32 map2_binormal          = 0x8447;
} // namespace values
STATICINLINE void binormal3b(i8 bx, i8 by, i8 bz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3bEXT)
    }
    glBinormal3bEXT(bx, by, bz);
    detail::error_check("Binormal3bEXT"sv);
}

template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void binormal3bv(span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3bvEXT)
    }
    glBinormal3bvEXT(
        v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("Binormal3bvEXT"sv);
}

STATICINLINE void binormal3d(f64 bx, f64 by, f64 bz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3dEXT)
    }
    glBinormal3dEXT(bx, by, bz);
    detail::error_check("Binormal3dEXT"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void binormal3dv(span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3dvEXT)
    }
    glBinormal3dvEXT(reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("Binormal3dvEXT"sv);
}

STATICINLINE void binormal3f(f32 bx, f32 by, f32 bz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3fEXT)
    }
    glBinormal3fEXT(bx, by, bz);
    detail::error_check("Binormal3fEXT"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void binormal3fv(span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3fvEXT)
    }
    glBinormal3fvEXT(reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("Binormal3fvEXT"sv);
}

STATICINLINE void binormal3i(i32 bx, i32 by, i32 bz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3iEXT)
    }
    glBinormal3iEXT(bx, by, bz);
    detail::error_check("Binormal3iEXT"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE void binormal3iv(span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3ivEXT)
    }
    glBinormal3ivEXT(reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("Binormal3ivEXT"sv);
}

STATICINLINE void binormal3s(i16 bx, i16 by, i16 bz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3sEXT)
    }
    glBinormal3sEXT(bx, by, bz);
    detail::error_check("Binormal3sEXT"sv);
}

template<class span_const_vec_3_i16>
requires(semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
             Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    STATICINLINE void binormal3sv(span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Binormal3svEXT)
    }
    glBinormal3svEXT(reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("Binormal3svEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void binormal_pointer(
        group::binormal_pointer_type_ext type,
        i32                              stride,
        span_const_void const&           pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BinormalPointerEXT)
    }
    glBinormalPointerEXT(
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("BinormalPointerEXT"sv);
}

STATICINLINE void tangent3b(i8 tx, i8 ty, i8 tz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3bEXT)
    }
    glTangent3bEXT(tx, ty, tz);
    detail::error_check("Tangent3bEXT"sv);
}

template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE void tangent3bv(span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3bvEXT)
    }
    glTangent3bvEXT(
        v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("Tangent3bvEXT"sv);
}

STATICINLINE void tangent3d(f64 tx, f64 ty, f64 tz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3dEXT)
    }
    glTangent3dEXT(tx, ty, tz);
    detail::error_check("Tangent3dEXT"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void tangent3dv(span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3dvEXT)
    }
    glTangent3dvEXT(reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("Tangent3dvEXT"sv);
}

STATICINLINE void tangent3f(f32 tx, f32 ty, f32 tz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3fEXT)
    }
    glTangent3fEXT(tx, ty, tz);
    detail::error_check("Tangent3fEXT"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void tangent3fv(span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3fvEXT)
    }
    glTangent3fvEXT(reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("Tangent3fvEXT"sv);
}

STATICINLINE void tangent3i(i32 tx, i32 ty, i32 tz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3iEXT)
    }
    glTangent3iEXT(tx, ty, tz);
    detail::error_check("Tangent3iEXT"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE void tangent3iv(span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3ivEXT)
    }
    glTangent3ivEXT(reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("Tangent3ivEXT"sv);
}

STATICINLINE void tangent3s(i16 tx, i16 ty, i16 tz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3sEXT)
    }
    glTangent3sEXT(tx, ty, tz);
    detail::error_check("Tangent3sEXT"sv);
}

template<class span_const_vec_3_i16>
requires(semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
             Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    STATICINLINE void tangent3sv(span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Tangent3svEXT)
    }
    glTangent3svEXT(reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("Tangent3svEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void tangent_pointer(
        group::tangent_pointer_type_ext type,
        i32                             stride,
        span_const_void const&          pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TangentPointerEXT)
    }
    glTangentPointerEXT(
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("TangentPointerEXT"sv);
}

} // namespace gl::ext::coordinate_frame
#endif // GL_EXT_coordinate_frame
namespace gl::ext::coordinate_frame {
constexpr auto name = "GL_EXT_coordinate_frame";
}
