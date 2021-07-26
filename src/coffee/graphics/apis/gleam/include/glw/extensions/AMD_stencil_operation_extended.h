#pragma once

#ifdef GL_AMD_stencil_operation_extended
namespace gl::amd::stencil_operation_extended {
namespace values {
constexpr libc_types::u32 set                   = 0x874A;
constexpr libc_types::u32 replace_value         = 0x874B;
constexpr libc_types::u32 stencil_op_value      = 0x874C;
constexpr libc_types::u32 stencil_back_op_value = 0x874D;
} // namespace values
STATICINLINE void stencil_op_value(
    group::stencil_face_direction face, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilOpValueAMD)
    }
    glStencilOpValueAMD(static_cast<GLenum>(face), value);
    detail::error_check("StencilOpValueAMD"sv);
}

} // namespace gl::amd::stencil_operation_extended
#endif // GL_AMD_stencil_operation_extended
namespace gl::amd::stencil_operation_extended {
constexpr auto name = "GL_AMD_stencil_operation_extended";
}
