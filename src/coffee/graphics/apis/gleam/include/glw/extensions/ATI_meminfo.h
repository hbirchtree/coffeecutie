#pragma once

#ifdef GL_ATI_meminfo
namespace gl::ati::meminfo {
namespace values {
constexpr u32 vbo_free_memory          = 0x87FB;
constexpr u32 texture_free_memory      = 0x87FC;
constexpr u32 renderbuffer_free_memory = 0x87FD;
} // namespace values
} // namespace gl::ati::meminfo
#endif // GL_ATI_meminfo
namespace gl::ati::meminfo {
constexpr auto name = "GL_ATI_meminfo";
}
