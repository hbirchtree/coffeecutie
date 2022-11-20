#pragma once

#ifdef GL_EXT_map_buffer_range
#include "../enums/BufferStorageMask.h"
#include "../enums/MapBufferAccessMask.h"
namespace gl::ext::map_buffer_range {
using gl::group::buffer_storage_mask;
using gl::group::map_buffer_access_mask;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_map_buffer_range
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
        GLW_FPTR_CHECK(FlushMappedBufferRangeEXT)
    }
    glFlushMappedBufferRangeEXT(static_cast<GLenum>(target), offset, length);
    detail::error_check("FlushMappedBufferRangeEXT"sv);
}

/*!
 * \brief Part of GL_EXT_map_buffer_range
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
        GLW_FPTR_CHECK(MapBufferRangeEXT)
    }
    auto out = glMapBufferRangeEXT(
        static_cast<GLenum>(target),
        offset,
        length,
        static_cast<GLenum>(access));
    detail::error_check("MapBufferRangeEXT"sv);
    return out;
}

} // namespace gl::ext::map_buffer_range
#endif // GL_EXT_map_buffer_range
namespace gl::ext::map_buffer_range {
constexpr auto name = "GL_EXT_map_buffer_range";
}
