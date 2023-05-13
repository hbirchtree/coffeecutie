#pragma once

#ifdef GL_ARB_texture_mirrored_repeat
namespace gl::arb::texture_mirrored_repeat {
namespace values {
constexpr u32 mirrored_repeat = 0x8370;
} // namespace values
} // namespace gl::arb::texture_mirrored_repeat
#endif // GL_ARB_texture_mirrored_repeat
namespace gl::arb::texture_mirrored_repeat {
constexpr auto name = "GL_ARB_texture_mirrored_repeat";
}
