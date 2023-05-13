#pragma once

#ifdef GL_NV_polygon_mode
#include "../enums/EvalMapsModeNV.h"
namespace gl::nv::polygon_mode {
using gl::group::eval_maps_mode_nv;
namespace values {
constexpr u32 polygon_mode         = 0x0B40;
constexpr u32 point                = 0x1B00;
constexpr u32 line                 = 0x1B01;
constexpr u32 polygon_offset_point = 0x2A01;
constexpr u32 polygon_offset_line  = 0x2A02;
} // namespace values
/*!
 * \brief Part of GL_NV_polygon_mode
 * \param face GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void polygon_mode(
    group::triangle_face face, group::polygon_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonModeNV)
    }
    glPolygonModeNV(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("PolygonModeNV"sv);
}

} // namespace gl::nv::polygon_mode
#endif // GL_NV_polygon_mode
namespace gl::nv::polygon_mode {
constexpr auto name = "GL_NV_polygon_mode";
}
