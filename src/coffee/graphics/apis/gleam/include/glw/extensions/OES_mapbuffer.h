#pragma once

#ifdef GL_OES_mapbuffer
#include "../enums/BufferAccessARB.h"
namespace gl::oes::mapbuffer {
using gl::group::buffer_access_arb;
namespace values {
constexpr libc_types::u32 buffer_access      = 0x88BB;
constexpr libc_types::u32 buffer_mapped      = 0x88BC;
constexpr libc_types::u32 buffer_map_pointer = 0x88BD;
} // namespace values
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_OES_mapbuffer
 * \param target GLenum
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_buffer_pointerv(
    group::buffer_target_arb       target,
    group::buffer_pointer_name_arb pname,
    span_void                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointervOES)
    }
    glGetBufferPointervOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointervOES"sv);
}

/*!
 * \brief Part of GL_OES_mapbuffer
 * \param target GLenum
 * \param access GLenum
 * \return void *
 */
STATICINLINE void* map_buffer(
    group::buffer_target_arb target, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferOES)
    }
    auto out = glMapBufferOES(
        static_cast<GLenum>(target), static_cast<GLenum>(access));
    detail::error_check("MapBufferOES"sv);
    return out;
}

/*!
 * \brief Part of GL_OES_mapbuffer
 * \param target GLenum
 * \return Boolean
 */
STATICINLINE GLboolean unmap_buffer(group::buffer_target_arb target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBufferOES)
    }
    auto out = glUnmapBufferOES(static_cast<GLenum>(target));
    detail::error_check("UnmapBufferOES"sv);
    return out;
}

} // namespace gl::oes::mapbuffer
#endif // GL_OES_mapbuffer
namespace gl::oes::mapbuffer {
constexpr auto name = "GL_OES_mapbuffer";
}
