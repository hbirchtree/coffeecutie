#pragma once

#ifdef GL_ARB_parallel_shader_compile
namespace gl::arb::parallel_shader_compile {
namespace values {
constexpr u32 max_shader_compiler_threads = 0x91B0;
constexpr u32 completion_status           = 0x91B1;
} // namespace values
/*!
 * \brief Part of GL_ARB_parallel_shader_compile
 * \param count GLuint
 * \return void
 */
STATICINLINE void max_shader_compiler_threads(u32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaxShaderCompilerThreadsARB)
    }
    glMaxShaderCompilerThreadsARB(count);
    detail::error_check("MaxShaderCompilerThreadsARB"sv);
}

} // namespace gl::arb::parallel_shader_compile
#endif // GL_ARB_parallel_shader_compile
namespace gl::arb::parallel_shader_compile {
constexpr auto name = "GL_ARB_parallel_shader_compile";
}
