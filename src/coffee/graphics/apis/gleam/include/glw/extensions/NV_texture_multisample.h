#pragma once

#ifdef GL_NV_texture_multisample
namespace gl::nv::texture_multisample {
namespace values {
constexpr u32 texture_coverage_samples = 0x9045;
constexpr u32 texture_color_samples    = 0x9046;
} // namespace values
template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param target GLenum
     * \param coverageSamples GLsizei
     * \param colorSamples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void tex_image_2d_multisample_coverage(
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
requires(concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param target GLenum
     * \param coverageSamples GLsizei
     * \param colorSamples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void tex_image_3d_multisample_coverage(
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
requires(concepts::size_2d<size_2_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param texture GLuint
     * \param target GLenum
     * \param coverageSamples GLsizei
     * \param colorSamples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void texture_image_2d_multisample_coverage(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
requires(concepts::size_2d<size_2_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param texture GLuint
     * \param target GLenum
     * \param samples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void texture_image_2d_multisample(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
requires(concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param texture GLuint
     * \param target GLenum
     * \param coverageSamples GLsizei
     * \param colorSamples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void texture_image_3d_multisample_coverage(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
requires(concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NV_texture_multisample
     * \param texture GLuint
     * \param target GLenum
     * \param samples GLsizei
     * \param internalFormat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param fixedSampleLocations GLboolean
     * \return void
     */
    STATICINLINE void texture_image_3d_multisample(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
