#pragma once

#ifdef GL_ARB_parallel_shader_compile
#include "../enums/ProgramPropertyARB.h"

namespace gl::arb::parallel_shader_compile {
using gl::group::program_property_arb;

namespace values {
constexpr u32 max_shader_compiler_threads = 0x91B0;
} // namespace values

/*!
 * \brief Part of GL_ARB_parallel_shader_compile
 * \param count GLuint
 * \return void
 */
STATICINLINE void max_shader_compiler_threads(
    u32 count, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaxShaderCompilerThreadsARB)
    }
    glMaxShaderCompilerThreadsARB(count);
    detail::error_check("MaxShaderCompilerThreadsARB"sv, check_errors);
}

} // namespace gl::arb::parallel_shader_compile
#endif // GL_ARB_parallel_shader_compile
namespace gl::arb::parallel_shader_compile {
constexpr auto name = "GL_ARB_parallel_shader_compile";
}
