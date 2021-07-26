#pragma once

#ifdef GL_EXT_blend_color
#include "../enums/GetPName.h"
namespace gl::ext::blend_color {
using gl::group::get_prop;
namespace values {
constexpr libc_types::u32 constant_color           = 0x8001;
constexpr libc_types::u32 one_minus_constant_color = 0x8002;
constexpr libc_types::u32 constant_alpha           = 0x8003;
constexpr libc_types::u32 one_minus_constant_alpha = 0x8004;
} // namespace values
template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColorEXT)
    }
    glBlendColorEXT(red.x(), red.y(), red.z(), red.w());
    detail::error_check("BlendColorEXT"sv);
}

} // namespace gl::ext::blend_color
#endif // GL_EXT_blend_color
namespace gl::ext::blend_color {
constexpr auto name = "GL_EXT_blend_color";
}
