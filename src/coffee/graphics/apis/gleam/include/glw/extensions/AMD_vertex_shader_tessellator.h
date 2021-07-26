#pragma once

#ifdef GL_AMD_vertex_shader_tessellator
namespace gl::amd::vertex_shader_tessellator {
namespace values {
constexpr libc_types::u32 sampler_buffer              = 0x9001;
constexpr libc_types::u32 int_sampler_buffer          = 0x9002;
constexpr libc_types::u32 unsigned_int_sampler_buffer = 0x9003;
constexpr libc_types::u32 tessellation_mode           = 0x9004;
constexpr libc_types::u32 tessellation_factor         = 0x9005;
constexpr libc_types::u32 discrete                    = 0x9006;
constexpr libc_types::u32 continuous                  = 0x9007;
} // namespace values
STATICINLINE void tessellation_factor(f32 factor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TessellationFactorAMD)
    }
    glTessellationFactorAMD(factor);
    detail::error_check("TessellationFactorAMD"sv);
}

STATICINLINE void tessellation_mode(GLenum mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TessellationModeAMD)
    }
    glTessellationModeAMD(mode);
    detail::error_check("TessellationModeAMD"sv);
}

} // namespace gl::amd::vertex_shader_tessellator
#endif // GL_AMD_vertex_shader_tessellator
namespace gl::amd::vertex_shader_tessellator {
constexpr auto name = "GL_AMD_vertex_shader_tessellator";
}
