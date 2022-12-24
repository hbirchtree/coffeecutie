#pragma once

#ifdef GL_EXT_external_buffer
namespace gl::ext::external_buffer {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_external_buffer
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param clientBuffer GLeglClientBufferEXT
 * \param flags GLbitfield
 * \return void
 */
STATICINLINE void buffer_storage_external(
    GLenum                     target,
    GLintptr                   offset,
    GLsizeiptr                 size,
    GLeglClientBufferEXT       clientBuffer,
    group::buffer_storage_mask flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferStorageExternalEXT)
    }
    glBufferStorageExternalEXT(
        target, offset, size, clientBuffer, static_cast<GLenum>(flags));
    detail::error_check("BufferStorageExternalEXT"sv);
}

/*!
 * \brief Part of GL_EXT_external_buffer
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param clientBuffer GLeglClientBufferEXT
 * \param flags GLbitfield
 * \return void
 */
STATICINLINE void named_buffer_storage_external(
    u32                        buffer,
    GLintptr                   offset,
    GLsizeiptr                 size,
    GLeglClientBufferEXT       clientBuffer,
    group::buffer_storage_mask flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorageExternalEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferStorageExternalEXT(
        buffer, offset, size, clientBuffer, static_cast<GLenum>(flags));
    detail::error_check("NamedBufferStorageExternalEXT"sv);
}

} // namespace gl::ext::external_buffer
#endif // GL_EXT_external_buffer
namespace gl::ext::external_buffer {
constexpr auto name = "GL_EXT_external_buffer";
}
