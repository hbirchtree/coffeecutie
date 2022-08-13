#pragma once

#ifdef GL_NV_texture_multisample
namespace gl::nv::texture_multisample {
namespace values {
constexpr libc_types::u32 texture_coverage_samples = 0x9045;
constexpr libc_types::u32 texture_color_samples    = 0x9046;
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_image_2d_multisample_coverage(
        group::texture_target target,
        i32                   coverageSamples,
        i32                   colorSamples,
        i32                   internalFormat,
        size_2_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage2DMultisampleCoverageNV)
    }
    glTexImage2DMultisampleCoverageNV(
        static_cast<GLenum>(target),
        coverageSamples,
        colorSamples,
        internalFormat,
        width[0],
        width[1],
        fixedSampleLocations);
    detail::error_check("TexImage2DMultisampleCoverageNV"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_image_3d_multisample_coverage(
        group::texture_target target,
        i32                   coverageSamples,
        i32                   colorSamples,
        i32                   internalFormat,
        size_3_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3DMultisampleCoverageNV)
    }
    glTexImage3DMultisampleCoverageNV(
        static_cast<GLenum>(target),
        coverageSamples,
        colorSamples,
        internalFormat,
        width[0],
        width[1],
        width[2],
        fixedSampleLocations);
    detail::error_check("TexImage3DMultisampleCoverageNV"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_image_2d_multisample_coverage(
        u32                   texture,
        group::texture_target target,
        i32                   coverageSamples,
        i32                   colorSamples,
        i32                   internalFormat,
        size_2_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage2DMultisampleCoverageNV)
        glIsTexture(texture);
    }
    glTextureImage2DMultisampleCoverageNV(
        texture,
        static_cast<GLenum>(target),
        coverageSamples,
        colorSamples,
        internalFormat,
        width[0],
        width[1],
        fixedSampleLocations);
    detail::error_check("TextureImage2DMultisampleCoverageNV"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_image_2d_multisample(
        u32                   texture,
        group::texture_target target,
        i32                   samples,
        i32                   internalFormat,
        size_2_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage2DMultisampleNV)
        glIsTexture(texture);
    }
    glTextureImage2DMultisampleNV(
        texture,
        static_cast<GLenum>(target),
        samples,
        internalFormat,
        width[0],
        width[1],
        fixedSampleLocations);
    detail::error_check("TextureImage2DMultisampleNV"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_image_3d_multisample_coverage(
        u32                   texture,
        group::texture_target target,
        i32                   coverageSamples,
        i32                   colorSamples,
        i32                   internalFormat,
        size_3_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage3DMultisampleCoverageNV)
        glIsTexture(texture);
    }
    glTextureImage3DMultisampleCoverageNV(
        texture,
        static_cast<GLenum>(target),
        coverageSamples,
        colorSamples,
        internalFormat,
        width[0],
        width[1],
        width[2],
        fixedSampleLocations);
    detail::error_check("TextureImage3DMultisampleCoverageNV"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_image_3d_multisample(
        u32                   texture,
        group::texture_target target,
        i32                   samples,
        i32                   internalFormat,
        size_3_i32 const&     width,
        bool                  fixedSampleLocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage3DMultisampleNV)
        glIsTexture(texture);
    }
    glTextureImage3DMultisampleNV(
        texture,
        static_cast<GLenum>(target),
        samples,
        internalFormat,
        width[0],
        width[1],
        width[2],
        fixedSampleLocations);
    detail::error_check("TextureImage3DMultisampleNV"sv);
}

} // namespace gl::nv::texture_multisample
#endif // GL_NV_texture_multisample
namespace gl::nv::texture_multisample {
constexpr auto name = "GL_NV_texture_multisample";
}