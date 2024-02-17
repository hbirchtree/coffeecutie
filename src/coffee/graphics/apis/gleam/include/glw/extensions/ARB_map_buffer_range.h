#pragma once

#ifdef GL_ARB_map_buffer_range
#include "../enums/BufferStorageMask.h"
#include "../enums/MapBufferAccessMask.h"

namespace gl::arb::map_buffer_range {
using gl::group::buffer_storage_mask;
using gl::group::map_buffer_access_mask;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_map_buffer_range
 * \param target GLenum
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \return void
 */
STATICINLINE void flush_mapped_buffer_range(
    group::buffer_target_arb target, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedBufferRange)
    }
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    detail::error_check("FlushMappedBufferRange"sv);
}

/*!
 * \brief Part of GL_ARB_map_buffer_range
 * \param target GLenum
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \param access GLbitfield
 * \return void *
 */
STATICINLINE void* map_buffer_range(
    group::buffer_target_arb      target,
    GLintptr                      offset,
    GLsizeiptr                    length,
    group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferRange)
    }
    auto out = glMapBufferRange(
        static_cast<GLenum>(target),
        offset,
        length,
        static_cast<GLenum>(access));
    detail::error_check("MapBufferRange"sv);
    return out;
}

} // namespace gl::arb::map_buffer_range
#endif // GL_ARB_map_buffer_range
namespace gl::arb::map_buffer_range {
constexpr auto name = "GL_ARB_map_buffer_range";
}
