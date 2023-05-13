#pragma once

#ifdef GL_NVX_gpu_memory_info
namespace gl::nvx::gpu_memory_info {
namespace values {
constexpr u32 gpu_memory_info_dedicated_vidmem         = 0x9047;
constexpr u32 gpu_memory_info_total_available_memory   = 0x9048;
constexpr u32 gpu_memory_info_current_available_vidmem = 0x9049;
constexpr u32 gpu_memory_info_eviction_count           = 0x904A;
constexpr u32 gpu_memory_info_evicted_memory           = 0x904B;
} // namespace values
} // namespace gl::nvx::gpu_memory_info
#endif // GL_NVX_gpu_memory_info
namespace gl::nvx::gpu_memory_info {
constexpr auto name = "GL_NVX_gpu_memory_info";
}
