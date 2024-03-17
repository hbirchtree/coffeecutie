#pragma once

#ifdef GL_EXT_polygon_offset
#include "../enums/GetPName.h"

namespace gl::ext::polygon_offset {
using gl::group::get_prop;

namespace values {
constexpr u32 polygon_offset        = 0x8037;
constexpr u32 polygon_offset_factor = 0x8038;
} // namespace values

/*!
 * \brief Part of GL_EXT_polygon_offset
 * \param factor GLfloat
 * \param bias GLfloat
 * \return void
 */
STATICINLINE void polygon_offset(
    f32 factor, f32 bias, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetEXT)
    }
    glPolygonOffsetEXT(factor, bias);
    detail::error_check("PolygonOffsetEXT"sv, check_errors);
}

} // namespace gl::ext::polygon_offset
#endif // GL_EXT_polygon_offset
namespace gl::ext::polygon_offset {
constexpr auto name = "GL_EXT_polygon_offset";
}
