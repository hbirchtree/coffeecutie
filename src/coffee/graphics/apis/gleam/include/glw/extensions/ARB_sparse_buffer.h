#pragma once

#ifdef GL_ARB_sparse_buffer
#include "../enums/BufferStorageMask.h"
namespace gl::arb::sparse_buffer {
using gl::group::buffer_storage_mask;
namespace values {
constexpr libc_types::u32 sparse_buffer_page_size = 0x82F8;
} // namespace values
/*!
 * \brief Part of GL_ARB_sparse_buffer
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void buffer_page_commitment(
    GLenum target, GLintptr offset, GLsizeiptr size, bool commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferPageCommitmentARB)
    }
    glBufferPageCommitmentARB(target, offset, size, commit);
    detail::error_check("BufferPageCommitmentARB"sv);
}

/*!
 * \brief Part of GL_ARB_sparse_buffer
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void named_buffer_page_commitment_ext(
    u32 buffer, GLintptr offset, GLsizeiptr size, bool commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferPageCommitmentEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferPageCommitmentEXT(buffer, offset, size, commit);
    detail::error_check("NamedBufferPageCommitmentEXT"sv);
}

/*!
 * \brief Part of GL_ARB_sparse_buffer
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void named_buffer_page_commitment(
    u32 buffer, GLintptr offset, GLsizeiptr size, bool commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferPageCommitmentARB)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferPageCommitmentARB(buffer, offset, size, commit);
    detail::error_check("NamedBufferPageCommitmentARB"sv);
}

} // namespace gl::arb::sparse_buffer
#endif // GL_ARB_sparse_buffer
namespace gl::arb::sparse_buffer {
constexpr auto name = "GL_ARB_sparse_buffer";
}
