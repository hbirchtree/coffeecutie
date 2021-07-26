#pragma once

#ifdef GL_NV_conservative_raster_dilate
namespace gl::nv::conservative_raster_dilate {
namespace values {
constexpr libc_types::u32 conservative_raster_dilate             = 0x9379;
constexpr libc_types::u32 conservative_raster_dilate_range       = 0x937A;
constexpr libc_types::u32 conservative_raster_dilate_granularity = 0x937B;
} // namespace values
STATICINLINE void conservative_raster_parameter(GLenum pname, f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConservativeRasterParameterfNV)
    }
    glConservativeRasterParameterfNV(pname, value);
    detail::error_check("ConservativeRasterParameterfNV"sv);
}

} // namespace gl::nv::conservative_raster_dilate
#endif // GL_NV_conservative_raster_dilate
namespace gl::nv::conservative_raster_dilate {
constexpr auto name = "GL_NV_conservative_raster_dilate";
}
