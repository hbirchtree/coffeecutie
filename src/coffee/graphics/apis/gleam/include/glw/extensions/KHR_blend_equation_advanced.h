#pragma once

#ifdef GL_KHR_blend_equation_advanced
namespace gl::khr::blend_equation_advanced {
namespace values {
constexpr u32 multiply       = 0x9294;
constexpr u32 screen         = 0x9295;
constexpr u32 overlay        = 0x9296;
constexpr u32 darken         = 0x9297;
constexpr u32 lighten        = 0x9298;
constexpr u32 colordodge     = 0x9299;
constexpr u32 colorburn      = 0x929A;
constexpr u32 hardlight      = 0x929B;
constexpr u32 softlight      = 0x929C;
constexpr u32 difference     = 0x929E;
constexpr u32 exclusion      = 0x92A0;
constexpr u32 hsl_hue        = 0x92AD;
constexpr u32 hsl_saturation = 0x92AE;
constexpr u32 hsl_color      = 0x92AF;
constexpr u32 hsl_luminosity = 0x92B0;
} // namespace values

/*!
 * \brief Part of GL_KHR_blend_equation_advanced

 * \return void
 */
STATICINLINE void blend_barrier(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendBarrierKHR)
    }
    glBlendBarrierKHR();
    detail::error_check("BlendBarrierKHR"sv, check_errors);
}

} // namespace gl::khr::blend_equation_advanced
#endif // GL_KHR_blend_equation_advanced
namespace gl::khr::blend_equation_advanced {
constexpr auto name = "GL_KHR_blend_equation_advanced";
}
