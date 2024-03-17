#pragma once

#ifdef GL_EXT_raster_multisample
namespace gl::ext::raster_multisample {
namespace values {
constexpr u32 raster_multisample                = 0x9327;
constexpr u32 raster_samples                    = 0x9328;
constexpr u32 max_raster_samples                = 0x9329;
constexpr u32 raster_fixed_sample_locations     = 0x932A;
constexpr u32 multisample_rasterization_allowed = 0x932B;
constexpr u32 effective_raster_samples          = 0x932C;
} // namespace values

/*!
 * \brief Part of GL_EXT_raster_multisample
 * \param samples GLuint
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void raster_samples(
    u32         samples,
    bool        fixedsamplelocations,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterSamplesEXT)
    }
    glRasterSamplesEXT(samples, fixedsamplelocations);
    detail::error_check("RasterSamplesEXT"sv, check_errors);
}

} // namespace gl::ext::raster_multisample
#endif // GL_EXT_raster_multisample
namespace gl::ext::raster_multisample {
constexpr auto name = "GL_EXT_raster_multisample";
}
