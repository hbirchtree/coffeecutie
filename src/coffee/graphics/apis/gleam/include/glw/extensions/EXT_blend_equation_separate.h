#pragma once

#ifdef GL_EXT_blend_equation_separate
namespace gl::ext::blend_equation_separate {
namespace values {
constexpr u32 blend_equation_rgb   = 0x8009;
constexpr u32 blend_equation_alpha = 0x883D;
} // namespace values

/*!
 * \brief Part of GL_EXT_blend_equation_separate
 * \param modeRGB GLenum
 * \param modeAlpha GLenum
 * \return void
 */
STATICINLINE void blend_equation_separate(
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparateEXT)
    }
    glBlendEquationSeparateEXT(
        static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparateEXT"sv, check_errors);
}

} // namespace gl::ext::blend_equation_separate
#endif // GL_EXT_blend_equation_separate
namespace gl::ext::blend_equation_separate {
constexpr auto name = "GL_EXT_blend_equation_separate";
}
