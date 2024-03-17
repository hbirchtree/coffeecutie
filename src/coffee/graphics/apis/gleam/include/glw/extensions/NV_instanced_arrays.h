#pragma once

#ifdef GL_NV_instanced_arrays
namespace gl::nv::instanced_arrays {
namespace values {
constexpr u32 vertex_attrib_array_divisor = 0x88FE;
} // namespace values

/*!
 * \brief Part of GL_NV_instanced_arrays
 * \param index GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_divisor(
    u32 index, u32 divisor, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisorNV)
    }
    glVertexAttribDivisorNV(index, divisor);
    detail::error_check("VertexAttribDivisorNV"sv, check_errors);
}

} // namespace gl::nv::instanced_arrays
#endif // GL_NV_instanced_arrays
namespace gl::nv::instanced_arrays {
constexpr auto name = "GL_NV_instanced_arrays";
}
