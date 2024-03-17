#pragma once

#ifdef GL_AMD_vertex_shader_tessellator
namespace gl::amd::vertex_shader_tessellator {
namespace values {
constexpr u32 sampler_buffer              = 0x9001;
constexpr u32 int_sampler_buffer          = 0x9002;
constexpr u32 unsigned_int_sampler_buffer = 0x9003;
constexpr u32 tessellation_mode           = 0x9004;
constexpr u32 tessellation_factor         = 0x9005;
constexpr u32 discrete                    = 0x9006;
constexpr u32 continuous                  = 0x9007;
} // namespace values

/*!
 * \brief Part of GL_AMD_vertex_shader_tessellator
 * \param factor GLfloat
 * \return void
 */
STATICINLINE void tessellation_factor(
    f32 factor, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TessellationFactorAMD)
    }
    glTessellationFactorAMD(factor);
    detail::error_check("TessellationFactorAMD"sv, check_errors);
}

/*!
 * \brief Part of GL_AMD_vertex_shader_tessellator
 * \param mode GLenum
 * \return void
 */
STATICINLINE void tessellation_mode(
    GLenum mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TessellationModeAMD)
    }
    glTessellationModeAMD(mode);
    detail::error_check("TessellationModeAMD"sv, check_errors);
}

} // namespace gl::amd::vertex_shader_tessellator
#endif // GL_AMD_vertex_shader_tessellator
namespace gl::amd::vertex_shader_tessellator {
constexpr auto name = "GL_AMD_vertex_shader_tessellator";
}
