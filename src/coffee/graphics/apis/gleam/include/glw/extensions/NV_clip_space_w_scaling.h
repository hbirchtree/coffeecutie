#pragma once

#ifdef GL_NV_clip_space_w_scaling
namespace gl::nv::clip_space_w_scaling {
namespace values {
constexpr libc_types::u32 viewport_position_w_scale         = 0x937C;
constexpr libc_types::u32 viewport_position_w_scale_x_coeff = 0x937D;
constexpr libc_types::u32 viewport_position_w_scale_y_coeff = 0x937E;
} // namespace values
STATICINLINE void viewport_position_w_scale(u32 index, f32 xcoeff, f32 ycoeff)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportPositionWScaleNV)
    }
    glViewportPositionWScaleNV(index, xcoeff, ycoeff);
    detail::error_check("ViewportPositionWScaleNV"sv);
}

} // namespace gl::nv::clip_space_w_scaling
#endif // GL_NV_clip_space_w_scaling
namespace gl::nv::clip_space_w_scaling {
constexpr auto name = "GL_NV_clip_space_w_scaling";
}
