#pragma once

#ifdef GL_EXT_buffer_storage
#include "../enums/BufferStorageMask.h"
#include "../enums/MapBufferAccessMask.h"
#include "../enums/MemoryBarrierMask.h"
namespace gl::ext::buffer_storage {
using gl::group::buffer_storage_mask;
using gl::group::map_buffer_access_mask;
using gl::group::memory_barrier_mask;
namespace values {
constexpr libc_types::u32 buffer_immutable_storage = 0x821F;
constexpr libc_types::u32 buffer_storage_flags     = 0x8220;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_storage(
        group::buffer_storage_target target,
        span_const_void const&       data,
        group::buffer_storage_mask   flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferStorageEXT)
    }
    glBufferStorageEXT(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("BufferStorageEXT"sv);
}

} // namespace gl::ext::buffer_storage
#endif // GL_EXT_buffer_storage
namespace gl::ext::buffer_storage {
constexpr auto name = "GL_EXT_buffer_storage";
}
