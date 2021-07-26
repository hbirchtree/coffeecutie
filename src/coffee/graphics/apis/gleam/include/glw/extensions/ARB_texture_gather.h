#pragma once

#ifdef GL_ARB_texture_gather
namespace gl::arb::texture_gather {
namespace values {
constexpr libc_types::u32 min_program_texture_gather_offset     = 0x8E5E;
constexpr libc_types::u32 max_program_texture_gather_offset     = 0x8E5F;
constexpr libc_types::u32 max_program_texture_gather_components = 0x8F9F;
} // namespace values
} // namespace gl::arb::texture_gather
#endif // GL_ARB_texture_gather
namespace gl::arb::texture_gather {
constexpr auto name = "GL_ARB_texture_gather";
}
