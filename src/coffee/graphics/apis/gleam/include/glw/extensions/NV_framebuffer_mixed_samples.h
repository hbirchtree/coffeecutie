#pragma once

#ifdef GL_NV_framebuffer_mixed_samples
namespace gl::nv::framebuffer_mixed_samples {
namespace values {
constexpr libc_types::u32 color_samples                         = 0x8E20;
constexpr libc_types::u32 raster_multisample_ext                = 0x9327;
constexpr libc_types::u32 raster_samples_ext                    = 0x9328;
constexpr libc_types::u32 max_raster_samples_ext                = 0x9329;
constexpr libc_types::u32 raster_fixed_sample_locations_ext     = 0x932A;
constexpr libc_types::u32 multisample_rasterization_allowed_ext = 0x932B;
constexpr libc_types::u32 effective_raster_samples_ext          = 0x932C;
constexpr libc_types::u32 depth_samples                         = 0x932D;
constexpr libc_types::u32 stencil_samples                       = 0x932E;
constexpr libc_types::u32 mixed_depth_samples_supported         = 0x932F;
constexpr libc_types::u32 mixed_stencil_samples_supported       = 0x9330;
constexpr libc_types::u32 coverage_modulation_table             = 0x9331;
constexpr libc_types::u32 coverage_modulation                   = 0x9332;
constexpr libc_types::u32 coverage_modulation_table_size        = 0x9333;
} // namespace values
/*!
 * \brief Part of GL_NV_framebuffer_mixed_samples
 * \param components GLenum
 * \return void
 */
STATICINLINE void coverage_modulation(GLenum components)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverageModulationNV)
    }
    glCoverageModulationNV(components);
    detail::error_check("CoverageModulationNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_framebuffer_mixed_samples
     * \param n GLsizei
     * \param v const GLfloat *
     * \return void
     */
    STATICINLINE void coverage_modulation_table(span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverageModulationTableNV)
    }
    glCoverageModulationTableNV(
        v.size(),
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("CoverageModulationTableNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_framebuffer_mixed_samples
     * \param bufSize GLsizei
     * \param v GLfloat *
     * \return void
     */
    STATICINLINE void get_coverage_modulation_table(i32 bufSize, span_f32 v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCoverageModulationTableNV)
    }
    glGetCoverageModulationTableNV(
        bufSize, v.size() ? reinterpret_cast<GLfloat*>(v.data()) : nullptr);
    detail::error_check("GetCoverageModulationTableNV"sv);
}

/*!
 * \brief Part of GL_NV_framebuffer_mixed_samples
 * \param samples GLuint
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void raster_samples_ext(u32 samples, bool fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterSamplesEXT)
    }
    glRasterSamplesEXT(samples, fixedsamplelocations);
    detail::error_check("RasterSamplesEXT"sv);
}

} // namespace gl::nv::framebuffer_mixed_samples
#endif // GL_NV_framebuffer_mixed_samples
namespace gl::nv::framebuffer_mixed_samples {
constexpr auto name = "GL_NV_framebuffer_mixed_samples";
}
