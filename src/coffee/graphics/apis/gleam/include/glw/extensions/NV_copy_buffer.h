#pragma once

#ifdef GL_NV_copy_buffer
namespace gl::nv::copy_buffer {
namespace values {
constexpr u32 copy_read_buffer  = 0x8F36;
constexpr u32 copy_write_buffer = 0x8F37;
} // namespace values

/*!
 * \brief Part of GL_NV_copy_buffer
 * \param readTarget GLenum
 * \param writeTarget GLenum
 * \param readOffset GLintptr
 * \param writeOffset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void copy_buffer_sub_data(
    group::copy_buffer_sub_data_target readTarget,
    group::copy_buffer_sub_data_target writeTarget,
    GLintptr                           readOffset,
    GLintptr                           writeOffset,
    GLsizeiptr                         size,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyBufferSubDataNV)
    }
    glCopyBufferSubDataNV(
        static_cast<GLenum>(readTarget),
        static_cast<GLenum>(writeTarget),
        readOffset,
        writeOffset,
        size);
    detail::error_check("CopyBufferSubDataNV"sv, check_errors);
}

} // namespace gl::nv::copy_buffer
#endif // GL_NV_copy_buffer
namespace gl::nv::copy_buffer {
constexpr auto name = "GL_NV_copy_buffer";
}
