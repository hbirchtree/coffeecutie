#pragma once

#ifdef GL_ARM_mali_program_binary
namespace gl::arm::mali_program_binary {
namespace values {
constexpr libc_types::u32 mali_program_binary = 0x8F61;
} // namespace values
} // namespace gl::arm::mali_program_binary
#endif // GL_ARM_mali_program_binary
namespace gl::arm::mali_program_binary {
constexpr auto name = "GL_ARM_mali_program_binary";
}
