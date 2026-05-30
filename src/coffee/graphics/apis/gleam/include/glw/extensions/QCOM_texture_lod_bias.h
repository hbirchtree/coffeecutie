#pragma once

#ifdef GL_QCOM_texture_lod_bias
namespace gl::qcom::texture_lod_bias {
namespace values {
constexpr u32 texture_lod_bias = 0x8C96;
} // namespace values
} // namespace gl::qcom::texture_lod_bias
#endif // GL_QCOM_texture_lod_bias
namespace gl::qcom::texture_lod_bias {
constexpr auto name = "GL_QCOM_texture_lod_bias";
}
