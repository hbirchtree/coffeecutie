#pragma once

#ifdef GL_KHR_context_flush_control
namespace gl::khr::context_flush_control {
namespace values {
#if defined(GL_VERSION_1_0)
constexpr u32 context_release_behavior = 0x82FB;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 context_release_behavior = 0x82FB;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 context_release_behavior_flush = 0x82FC;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 context_release_behavior_flush = 0x82FC;
#endif
} // namespace values
} // namespace gl::khr::context_flush_control
#endif // GL_KHR_context_flush_control
namespace gl::khr::context_flush_control {
constexpr auto name = "GL_KHR_context_flush_control";
}
