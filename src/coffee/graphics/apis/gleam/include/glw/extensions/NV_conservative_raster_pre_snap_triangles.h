#pragma once

#ifdef GL_NV_conservative_raster_pre_snap_triangles
namespace gl::nv::conservative_raster_pre_snap_triangles {
namespace values {
constexpr libc_types::u32 conservative_raster_mode                    = 0x954D;
constexpr libc_types::u32 conservative_raster_mode_post_snap          = 0x954E;
constexpr libc_types::u32 conservative_raster_mode_pre_snap_triangles = 0x954F;
} // namespace values
/*!
 * \brief Part of GL_NV_conservative_raster_pre_snap_triangles
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void conservative_raster_parameter(GLenum pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConservativeRasterParameteriNV)
    }
    glConservativeRasterParameteriNV(pname, param);
    detail::error_check("ConservativeRasterParameteriNV"sv);
}

} // namespace gl::nv::conservative_raster_pre_snap_triangles
#endif // GL_NV_conservative_raster_pre_snap_triangles
namespace gl::nv::conservative_raster_pre_snap_triangles {
constexpr auto name = "GL_NV_conservative_raster_pre_snap_triangles";
}
