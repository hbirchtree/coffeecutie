#pragma once

#ifdef GL_NV_conservative_raster
namespace gl::nv::conservative_raster {
namespace values {
constexpr libc_types::u32 conservative_rasterization       = 0x9346;
constexpr libc_types::u32 subpixel_precision_bias_x_bits   = 0x9347;
constexpr libc_types::u32 subpixel_precision_bias_y_bits   = 0x9348;
constexpr libc_types::u32 max_subpixel_precision_bias_bits = 0x9349;
} // namespace values
STATICINLINE void subpixel_precision_bias(u32 xbits, u32 ybits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SubpixelPrecisionBiasNV)
    }
    glSubpixelPrecisionBiasNV(xbits, ybits);
    detail::error_check("SubpixelPrecisionBiasNV"sv);
}

} // namespace gl::nv::conservative_raster
#endif // GL_NV_conservative_raster
namespace gl::nv::conservative_raster {
constexpr auto name = "GL_NV_conservative_raster";
}
