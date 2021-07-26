#pragma once

#ifdef GL_NV_texture_expand_normal
namespace gl::nv::texture_expand_normal {
namespace values {
constexpr libc_types::u32 texture_unsigned_remap_mode = 0x888F;
} // namespace values
} // namespace gl::nv::texture_expand_normal
#endif // GL_NV_texture_expand_normal
namespace gl::nv::texture_expand_normal {
constexpr auto name = "GL_NV_texture_expand_normal";
}
