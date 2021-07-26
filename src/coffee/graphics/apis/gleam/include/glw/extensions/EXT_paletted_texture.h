#pragma once

#ifdef GL_EXT_paletted_texture
namespace gl::ext::paletted_texture {
namespace values {
constexpr libc_types::u32 color_index1       = 0x80E2;
constexpr libc_types::u32 color_index2       = 0x80E3;
constexpr libc_types::u32 color_index4       = 0x80E4;
constexpr libc_types::u32 color_index8       = 0x80E5;
constexpr libc_types::u32 color_index12      = 0x80E6;
constexpr libc_types::u32 color_index16      = 0x80E7;
constexpr libc_types::u32 texture_index_size = 0x80ED;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void color_table(
        group::color_table_target target,
        group::internal_format    internalFormat,
        i32                       width,
        group::pixel_format       format,
        group::pixel_type         type,
        span_const_void const&    table)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorTableEXT)
    }
    glColorTableEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalFormat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() ? reinterpret_cast<const void*>(table.data()) : nullptr);
    detail::error_check("ColorTableEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE void get_color_table(
    group::color_table_target target,
    group::pixel_format       format,
    group::pixel_type         type,
    span_void                 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTableEXT)
    }
    glGetColorTableEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetColorTableEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_color_table_parameter(
        group::color_table_target                 target,
        group::get_color_table_parameter_prop_sgi pname,
        span_f32                                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTableParameterfvEXT)
    }
    glGetColorTableParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_color_table_parameter(
        group::color_table_target                 target,
        group::get_color_table_parameter_prop_sgi pname,
        span_i32                                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTableParameterivEXT)
    }
    glGetColorTableParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameterivEXT"sv);
}

} // namespace gl::ext::paletted_texture
#endif // GL_EXT_paletted_texture
namespace gl::ext::paletted_texture {
constexpr auto name = "GL_EXT_paletted_texture";
}
