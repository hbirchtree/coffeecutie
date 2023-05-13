#pragma once

#ifdef GL_NV_alpha_to_coverage_dither_control
namespace gl::nv::alpha_to_coverage_dither_control {
namespace values {
constexpr u32 alpha_to_coverage_dither_mode    = 0x92BF;
constexpr u32 alpha_to_coverage_dither_default = 0x934D;
constexpr u32 alpha_to_coverage_dither_enable  = 0x934E;
constexpr u32 alpha_to_coverage_dither_disable = 0x934F;
} // namespace values
/*!
 * \brief Part of GL_NV_alpha_to_coverage_dither_control
 * \param mode GLenum
 * \return void
 */
STATICINLINE void alpha_to_coverage_dither_control(GLenum mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AlphaToCoverageDitherControlNV)
    }
    glAlphaToCoverageDitherControlNV(mode);
    detail::error_check("AlphaToCoverageDitherControlNV"sv);
}

} // namespace gl::nv::alpha_to_coverage_dither_control
#endif // GL_NV_alpha_to_coverage_dither_control
namespace gl::nv::alpha_to_coverage_dither_control {
constexpr auto name = "GL_NV_alpha_to_coverage_dither_control";
}
