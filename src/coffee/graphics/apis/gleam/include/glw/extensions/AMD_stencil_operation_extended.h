#pragma once

#ifdef GL_AMD_stencil_operation_extended
namespace gl::amd::stencil_operation_extended {
namespace values {
constexpr u32 set                   = 0x874A;
constexpr u32 replace_value         = 0x874B;
constexpr u32 stencil_op_value      = 0x874C;
constexpr u32 stencil_back_op_value = 0x874D;
} // namespace values

/*!
 * \brief Part of GL_AMD_stencil_operation_extended
 * \param face GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void stencil_op_value(
    group::triangle_face face,
    u32                  value,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilOpValueAMD)
    }
    glStencilOpValueAMD(static_cast<GLenum>(face), value);
    detail::error_check("StencilOpValueAMD"sv, check_errors);
}

} // namespace gl::amd::stencil_operation_extended
#endif // GL_AMD_stencil_operation_extended
namespace gl::amd::stencil_operation_extended {
constexpr auto name = "GL_AMD_stencil_operation_extended";
}
