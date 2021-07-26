#pragma once

#ifdef GL_NV_uniform_buffer_unified_memory
namespace gl::nv::uniform_buffer_unified_memory {
namespace values {
constexpr libc_types::u32 uniform_buffer_unified = 0x936E;
constexpr libc_types::u32 uniform_buffer_address = 0x936F;
constexpr libc_types::u32 uniform_buffer_length  = 0x9370;
} // namespace values
} // namespace gl::nv::uniform_buffer_unified_memory
#endif // GL_NV_uniform_buffer_unified_memory
namespace gl::nv::uniform_buffer_unified_memory {
constexpr auto name = "GL_NV_uniform_buffer_unified_memory";
}
