#pragma once

#ifdef GL_ARB_copy_buffer
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
namespace gl::arb::copy_buffer {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_copy_buffer
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
    GLsizeiptr                         size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyBufferSubData)
    }
    glCopyBufferSubData(
        static_cast<GLenum>(readTarget),
        static_cast<GLenum>(writeTarget),
        readOffset,
        writeOffset,
        size);
    detail::error_check("CopyBufferSubData"sv);
}

} // namespace gl::arb::copy_buffer
#endif // GL_ARB_copy_buffer
namespace gl::arb::copy_buffer {
constexpr auto name = "GL_ARB_copy_buffer";
}
