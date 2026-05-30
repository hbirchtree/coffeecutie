#pragma once

#ifdef GL_ARM_shader_core_properties
#include "../enums/GetPName.h"

namespace gl::arm::shader_core_properties {
using gl::group::get_prop;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARM_shader_core_properties
 * \param count GLuint
 * \return void
 */
STATICINLINE void max_active_shader_cores(
    u32 count, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaxActiveShaderCoresARM)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMaxActiveShaderCoresARM, count);
#endif
    glMaxActiveShaderCoresARM(count);
    detail::error_check("MaxActiveShaderCoresARM"sv, check_errors);
}

} // namespace gl::arm::shader_core_properties
#endif // GL_ARM_shader_core_properties
namespace gl::arm::shader_core_properties {
constexpr auto name = "GL_ARM_shader_core_properties";
}
