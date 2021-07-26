#pragma once

#ifdef GL_NV_texgen_reflection
namespace gl::nv::texgen_reflection {
namespace values {
constexpr libc_types::u32 normal_map     = 0x8511;
constexpr libc_types::u32 reflection_map = 0x8512;
} // namespace values
} // namespace gl::nv::texgen_reflection
#endif // GL_NV_texgen_reflection
namespace gl::nv::texgen_reflection {
constexpr auto name = "GL_NV_texgen_reflection";
}
