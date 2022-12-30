#pragma once

#ifdef GL_ARB_buffer_storage
#include "../enums/BufferPNameARB.h"
#include "../enums/BufferStorageMask.h"
#include "../enums/MapBufferAccessMask.h"
#include "../enums/MemoryBarrierMask.h"
namespace gl::arb::buffer_storage {
using gl::group::buffer_prop_arb;
using gl::group::buffer_storage_mask;
using gl::group::map_buffer_access_mask;
using gl::group::memory_barrier_mask;
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_buffer_storage
 * \param target GLenum
 * \param size GLsizeiptr
 * \param data const void *
 * \param flags GLbitfield
 * \return void
 */
STATICINLINE void buffer_storage(
    group::buffer_storage_target target,
    span_const_void const&       data,
    group::buffer_storage_mask   flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferStorage)
    }
    glBufferStorage(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("BufferStorage"sv);
}

} // namespace gl::arb::buffer_storage
#endif // GL_ARB_buffer_storage
namespace gl::arb::buffer_storage {
constexpr auto name = "GL_ARB_buffer_storage";
}
