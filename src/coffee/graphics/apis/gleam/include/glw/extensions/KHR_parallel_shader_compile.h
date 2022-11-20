#pragma once

#ifdef GL_KHR_parallel_shader_compile
namespace gl::khr::parallel_shader_compile {
namespace values {
constexpr libc_types::u32 max_shader_compiler_threads = 0x91B0;
constexpr libc_types::u32 completion_status           = 0x91B1;
} // namespace values
/*!
 * \brief Part of GL_KHR_parallel_shader_compile
 * \param count GLuint
 * \return void
 */
STATICINLINE void max_shader_compiler_threads(u32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaxShaderCompilerThreadsKHR)
    }
    glMaxShaderCompilerThreadsKHR(count);
    detail::error_check("MaxShaderCompilerThreadsKHR"sv);
}

} // namespace gl::khr::parallel_shader_compile
#endif // GL_KHR_parallel_shader_compile
namespace gl::khr::parallel_shader_compile {
constexpr auto name = "GL_KHR_parallel_shader_compile";
}
