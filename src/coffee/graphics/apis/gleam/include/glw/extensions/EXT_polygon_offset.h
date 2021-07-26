#pragma once

#ifdef GL_EXT_polygon_offset
#include "../enums/GetPName.h"
namespace gl::ext::polygon_offset {
using gl::group::get_prop;
namespace values {
constexpr libc_types::u32 polygon_offset        = 0x8037;
constexpr libc_types::u32 polygon_offset_factor = 0x8038;
} // namespace values
STATICINLINE void polygon_offset(f32 factor, f32 bias)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetEXT)
    }
    glPolygonOffsetEXT(factor, bias);
    detail::error_check("PolygonOffsetEXT"sv);
}

} // namespace gl::ext::polygon_offset
#endif // GL_EXT_polygon_offset
namespace gl::ext::polygon_offset {
constexpr auto name = "GL_EXT_polygon_offset";
}
