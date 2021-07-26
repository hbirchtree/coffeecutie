#pragma once

#ifdef GL_ARB_query_buffer_object
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/MemoryBarrierMask.h"
#include "../enums/QueryObjectParameterName.h"
namespace gl::arb::query_buffer_object {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::memory_barrier_mask;
using gl::group::query_object_parameter_name;
namespace values {
constexpr libc_types::u32 query_buffer_binding = 0x9193;
} // namespace values
} // namespace gl::arb::query_buffer_object
#endif // GL_ARB_query_buffer_object
namespace gl::arb::query_buffer_object {
constexpr auto name = "GL_ARB_query_buffer_object";
}
