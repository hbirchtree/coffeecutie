#pragma once

#ifdef GL_INTEL_blackhole_render
namespace gl::intel::blackhole_render {
namespace values {
constexpr u32 blackhole_render = 0x83FC;
} // namespace values
} // namespace gl::intel::blackhole_render
#endif // GL_INTEL_blackhole_render
namespace gl::intel::blackhole_render {
constexpr auto name = "GL_INTEL_blackhole_render";
}
