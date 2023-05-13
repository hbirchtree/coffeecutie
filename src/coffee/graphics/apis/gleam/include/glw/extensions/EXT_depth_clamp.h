#pragma once

#ifdef GL_EXT_depth_clamp
namespace gl::ext::depth_clamp {
namespace values {
constexpr u32 depth_clamp = 0x864F;
} // namespace values
} // namespace gl::ext::depth_clamp
#endif // GL_EXT_depth_clamp
namespace gl::ext::depth_clamp {
constexpr auto name = "GL_EXT_depth_clamp";
}
