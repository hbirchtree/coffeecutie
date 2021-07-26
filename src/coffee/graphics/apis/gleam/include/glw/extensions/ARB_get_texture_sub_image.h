#pragma once

#ifdef GL_ARB_get_texture_sub_image
namespace gl::arb::get_texture_sub_image {
namespace values {
} // namespace values
template<class size_3_i32, class span_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_texture_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        i32               bufSize,
        span_void         pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureSubImage)
        glIsTexture(texture);
    }
    glGetCompressedTextureSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureSubImage"sv);
}

template<class size_3_i32, class span_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_void>) STATICINLINE
    void get_texture_sub_image(
        u32                 texture,
        i32                 level,
        vec_3_i32 const&    xoffset,
        size_3_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        i32                 bufSize,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSubImage)
        glIsTexture(texture);
    }
    glGetTextureSubImage(
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
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureSubImage"sv);
}

} // namespace gl::arb::get_texture_sub_image
#endif // GL_ARB_get_texture_sub_image
namespace gl::arb::get_texture_sub_image {
constexpr auto name = "GL_ARB_get_texture_sub_image";
}
