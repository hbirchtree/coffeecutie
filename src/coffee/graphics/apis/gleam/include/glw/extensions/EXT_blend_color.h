#pragma once

#ifdef GL_EXT_blend_color
#include "../enums/GetPName.h"
namespace gl::ext::blend_color {
using gl::group::get_prop;
namespace values {
constexpr u32 constant_color           = 0x8001;
constexpr u32 one_minus_constant_color = 0x8002;
constexpr u32 constant_alpha           = 0x8003;
constexpr u32 one_minus_constant_alpha = 0x8004;
} // namespace values
template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
    /*!
     * \brief Part of GL_EXT_blend_color
     * \param red GLfloat
     * \param green GLfloat
     * \param blue GLfloat
     * \param alpha GLfloat
     * \return void
     */
    STATICINLINE void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColorEXT)
    }
    glBlendColorEXT(red[0], red[1], red[2], red[3]);
    detail::error_check("BlendColorEXT"sv);
}

} // namespace gl::ext::blend_color
#endif // GL_EXT_blend_color
namespace gl::ext::blend_color {
constexpr auto name = "GL_EXT_blend_color";
}
