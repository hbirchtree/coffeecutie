#pragma once

#ifdef GL_IMG_program_binary
namespace gl::img::program_binary {
namespace values {
constexpr u32 sgx_program_binary = 0x9130;
} // namespace values
} // namespace gl::img::program_binary
#endif // GL_IMG_program_binary
namespace gl::img::program_binary {
constexpr auto name = "GL_IMG_program_binary";
}
