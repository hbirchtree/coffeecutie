#pragma once

#ifdef GL_KHR_blend_equation_advanced
namespace gl::khr::blend_equation_advanced {
namespace values {
constexpr libc_types::u32 multiply       = 0x9294;
constexpr libc_types::u32 screen         = 0x9295;
constexpr libc_types::u32 overlay        = 0x9296;
constexpr libc_types::u32 darken         = 0x9297;
constexpr libc_types::u32 lighten        = 0x9298;
constexpr libc_types::u32 colordodge     = 0x9299;
constexpr libc_types::u32 colorburn      = 0x929A;
constexpr libc_types::u32 hardlight      = 0x929B;
constexpr libc_types::u32 softlight      = 0x929C;
constexpr libc_types::u32 difference     = 0x929E;
constexpr libc_types::u32 exclusion      = 0x92A0;
constexpr libc_types::u32 hsl_hue        = 0x92AD;
constexpr libc_types::u32 hsl_saturation = 0x92AE;
constexpr libc_types::u32 hsl_color      = 0x92AF;
constexpr libc_types::u32 hsl_luminosity = 0x92B0;
} // namespace values
/*!
 * \brief Part of GL_KHR_blend_equation_advanced

 * \return void
 */
STATICINLINE void blend_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendBarrierKHR)
    }
    glBlendBarrierKHR();
    detail::error_check("BlendBarrierKHR"sv);
}

} // namespace gl::khr::blend_equation_advanced
#endif // GL_KHR_blend_equation_advanced
namespace gl::khr::blend_equation_advanced {
constexpr auto name = "GL_KHR_blend_equation_advanced";
}
