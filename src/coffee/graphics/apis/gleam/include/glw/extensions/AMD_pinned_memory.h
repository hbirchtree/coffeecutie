#pragma once

#ifdef GL_AMD_pinned_memory
namespace gl::amd::pinned_memory {
namespace values {
constexpr libc_types::u32 external_virtual_memory_buffer = 0x9160;
} // namespace values
} // namespace gl::amd::pinned_memory
#endif // GL_AMD_pinned_memory
namespace gl::amd::pinned_memory {
constexpr auto name = "GL_AMD_pinned_memory";
}
