#pragma once

#ifdef GL_NV_shader_buffer_store
#include "../enums/BufferAccessARB.h"
#include "../enums/MemoryBarrierMask.h"
namespace gl::nv::shader_buffer_store {
using gl::group::buffer_access_arb;
using gl::group::memory_barrier_mask;
namespace values {
} // namespace values
} // namespace gl::nv::shader_buffer_store
#endif // GL_NV_shader_buffer_store
namespace gl::nv::shader_buffer_store {
constexpr auto name = "GL_NV_shader_buffer_store";
}
