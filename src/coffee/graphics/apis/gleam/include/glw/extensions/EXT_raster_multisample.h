#pragma once

#ifdef GL_EXT_raster_multisample
namespace gl::ext::raster_multisample {
namespace values {
constexpr libc_types::u32 raster_multisample                = 0x9327;
constexpr libc_types::u32 raster_samples                    = 0x9328;
constexpr libc_types::u32 max_raster_samples                = 0x9329;
constexpr libc_types::u32 raster_fixed_sample_locations     = 0x932A;
constexpr libc_types::u32 multisample_rasterization_allowed = 0x932B;
constexpr libc_types::u32 effective_raster_samples          = 0x932C;
} // namespace values
STATICINLINE void raster_samples(u32 samples, bool fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterSamplesEXT)
    }
    glRasterSamplesEXT(samples, fixedsamplelocations);
    detail::error_check("RasterSamplesEXT"sv);
}

} // namespace gl::ext::raster_multisample
#endif // GL_EXT_raster_multisample
namespace gl::ext::raster_multisample {
constexpr auto name = "GL_EXT_raster_multisample";
}
