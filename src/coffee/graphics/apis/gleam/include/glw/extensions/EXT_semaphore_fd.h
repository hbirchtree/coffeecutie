#pragma once

#ifdef GL_EXT_semaphore_fd
#include "../enums/ExternalHandleType.h"
namespace gl::ext::semaphore_fd {
using gl::group::external_handle_type;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_semaphore_fd
 * \param semaphore GLuint
 * \param handleType GLenum
 * \param fd GLint
 * \return void
 */
STATICINLINE void import_semaphore_fd(
    u32 semaphore, group::external_handle_type handleType, i32 fd)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportSemaphoreFdEXT)
    }
    glImportSemaphoreFdEXT(semaphore, static_cast<GLenum>(handleType), fd);
    detail::error_check("ImportSemaphoreFdEXT"sv);
}

} // namespace gl::ext::semaphore_fd
#endif // GL_EXT_semaphore_fd
namespace gl::ext::semaphore_fd {
constexpr auto name = "GL_EXT_semaphore_fd";
}
