#pragma once

#ifdef GL_EXT_memory_object_fd
#include "../enums/ExternalHandleType.h"

namespace gl::ext::memory_object_fd {
using gl::group::external_handle_type;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_memory_object_fd
 * \param memory GLuint
 * \param size GLuint64
 * \param handleType GLenum
 * \param fd GLint
 * \return void
 */
STATICINLINE void import_memory_fd(
    u32                         memory,
    u64                         size,
    group::external_handle_type handleType,
    i32                         fd,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportMemoryFdEXT)
    }
    glImportMemoryFdEXT(memory, size, static_cast<GLenum>(handleType), fd);
    detail::error_check("ImportMemoryFdEXT"sv, check_errors);
}

} // namespace gl::ext::memory_object_fd
#endif // GL_EXT_memory_object_fd
namespace gl::ext::memory_object_fd {
constexpr auto name = "GL_EXT_memory_object_fd";
}
