#pragma once

#ifdef GL_NV_fragment_coverage_to_color
namespace gl::nv::fragment_coverage_to_color {
namespace values {
constexpr u32 fragment_coverage_to_color = 0x92DD;
constexpr u32 fragment_coverage_color    = 0x92DE;
} // namespace values

/*!
 * \brief Part of GL_NV_fragment_coverage_to_color
 * \param color GLuint
 * \return void
 */
STATICINLINE void fragment_coverage_color(
    u32 color, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FragmentCoverageColorNV)
    }
    glFragmentCoverageColorNV(color);
    detail::error_check("FragmentCoverageColorNV"sv, check_errors);
}

} // namespace gl::nv::fragment_coverage_to_color
#endif // GL_NV_fragment_coverage_to_color
namespace gl::nv::fragment_coverage_to_color {
constexpr auto name = "GL_NV_fragment_coverage_to_color";
}
