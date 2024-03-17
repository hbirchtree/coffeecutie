#pragma once

#ifdef GL_NV_primitive_restart
namespace gl::nv::primitive_restart {
namespace values {
constexpr u32 primitive_restart       = 0x8558;
constexpr u32 primitive_restart_index = 0x8559;
} // namespace values

/*!
 * \brief Part of GL_NV_primitive_restart
 * \param index GLuint
 * \return void
 */
STATICINLINE void primitive_restart_index(
    u32 index, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartIndexNV)
    }
    glPrimitiveRestartIndexNV(index);
    detail::error_check("PrimitiveRestartIndexNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_primitive_restart

 * \return void
 */
STATICINLINE void primitive_restart(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartNV)
    }
    glPrimitiveRestartNV();
    detail::error_check("PrimitiveRestartNV"sv, check_errors);
}

} // namespace gl::nv::primitive_restart
#endif // GL_NV_primitive_restart
namespace gl::nv::primitive_restart {
constexpr auto name = "GL_NV_primitive_restart";
}
