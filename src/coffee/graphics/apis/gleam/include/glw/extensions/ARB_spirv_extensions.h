#pragma once

#ifdef GL_ARB_spirv_extensions
namespace gl::arb::spirv_extensions {
namespace values {
constexpr u32 spir_v_extensions     = 0x9553;
constexpr u32 num_spir_v_extensions = 0x9554;
} // namespace values
} // namespace gl::arb::spirv_extensions
#endif // GL_ARB_spirv_extensions
namespace gl::arb::spirv_extensions {
constexpr auto name = "GL_ARB_spirv_extensions";
}
